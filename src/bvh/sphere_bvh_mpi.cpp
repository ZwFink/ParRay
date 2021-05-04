#include "bvh.hpp"
#include <mpi.h>
#include "data_porting.h"
#include "vec3.h"
#include "camera.h"
#include <vector>
#include "color.h"
#include <ctime>
#include "boundable.h"
#include <omp.h>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "ray_tracing.h"
#define RENDER_COMPLETE std::numeric_limits<int>::min()

// a global distributor that processes request work from
void work_distributor_loop(const traceConfig& config,
                           int minimum_assignment
                           )
{
  const int total_rows = config.height;
  // one processor is the work distributor, will not be doing any work
  const int num_procs = config.numProcs - 1;
  int remaining_rows = total_rows;
  int finish_messages_distributed = 0;
  int incoming_request_buf = 0;
  int outgoing_request_buf[2];
  int last_row_assigned = 0;

  if(num_procs == 1)
    minimum_assignment = total_rows;

  while(finish_messages_distributed < num_procs)
    {
      // receive a work request from some process
      MPI_Recv(&incoming_request_buf,
               1, MPI_INT,
               MPI_ANY_SOURCE,
               MPI_ANY_TAG,
               MPI_COMM_WORLD,
               MPI_STATUS_IGNORE
               );

      int requesting_process = incoming_request_buf;
      // calculate the work that should be distributed
      int num_rows = remaining_rows / (2*num_procs);

      if(num_rows > 0)
        {
          if(num_rows < minimum_assignment)
            {
              num_rows = minimum_assignment;
            }
          outgoing_request_buf[0] = last_row_assigned;
          // each process does inclusive render of rows assigned to it
          outgoing_request_buf[1] = last_row_assigned + num_rows - 1;
          last_row_assigned += num_rows;
          remaining_rows -= num_rows;
        }
      else
        {
          outgoing_request_buf[0] = 0;
          outgoing_request_buf[1] = 0;
          finish_messages_distributed++;
        }

      MPI_Send(outgoing_request_buf,
               2, MPI_INT,
               requesting_process, 0,
               MPI_COMM_WORLD
               );
    }
}

void request_work(int myRank,
                  std::atomic_int& row_iter,
                  std::atomic_int& minimum_row
                  )
{

  // there is more work to be done
  int requestor = myRank;
  int received_data[2];

  MPI_Send(&requestor,
           1, MPI_INT,
           0, 0,
           MPI_COMM_WORLD
           );
  MPI_Recv(received_data,
           2, MPI_INT,
           0, 0,
           MPI_COMM_WORLD,
           MPI_STATUS_IGNORE
           );

  if(received_data[0] == 0 && received_data[1] == 0)
    {
      row_iter = RENDER_COMPLETE;
      minimum_row = RENDER_COMPLETE;
    }
  else
    {
      row_iter = received_data[1];
      minimum_row = received_data[0];
    }
}


void render_loop(const traceConfig& config, color *output_image,
                 std::atomic_int& row_iter, std::atomic_int& row_end,
                 std::atomic<bool>& requestor_complete,
                 std::atomic<bool>& render_complete,
                 std::condition_variable& get_work_var,
                 std::mutex& wait_mutex,
                 std::atomic_int& prev_row_start,
                 std::atomic_int& prev_row_end,
                 std::atomic_int& threads_seen,
                 int num_threads,
                 MPI_Datatype &color_type,
                 MPI_Win &window
                 )
{
  const camera& cam = config.cam;
  const int samples_per_pixel = config.samplePerPixel;
  const int image_width = config.width;
  const int image_height = config.height;
  const int max_depth = config.traceDepth;
  bool was_requesting_thread = false;
  int last_row_end = prev_row_end;
  int last_row_start = prev_row_start;
  BVH &world = config.world;

  int my_iter = 0;

 RENDER_LOOP:
  my_iter = row_iter--;
  while(my_iter >= row_end && my_iter > 0)
    {
  if(render_complete)
    {
      requestor_complete = true;
      get_work_var.notify_all();
    return;
    }

      for(int i = 0; i < image_width; i++)
        {
          color pixel_color(0, 0, 0);
          for(int s = 0; s < samples_per_pixel; ++s)
            {
              auto u = (i + random_double()) / (image_width - 1);
              auto v = (my_iter + random_double()) / (image_height - 1);

              ray r = cam.get_ray(u, v);
              pixel_color += ray_color(r, world, max_depth);
            }
          int access_idx = ((image_height - 1 - my_iter)*image_width + i);

          if(access_idx < 0)
            std::cerr << "BAD! Access too small! " << access_idx << std::endl;;

          if(access_idx >= image_width*image_height)
            std::cerr << "BAD! Access too big! " << access_idx << " " << image_width * image_height << " " << my_iter << std::endl;;
          output_image[((image_height - 1 - my_iter)*image_width + i)] = pixel_color;
        }

      my_iter = row_iter--;
    }

  if(my_iter == row_end - 1)
    {
      requestor_complete = false;
      was_requesting_thread = true;
      last_row_end = prev_row_end;
      last_row_start = prev_row_start;

      request_work(config.myRank,
                   row_iter,
                   row_end
                   );

      int prev_end = row_end;
      int prev_start = row_iter;

      prev_row_end = prev_end;
      prev_row_start = prev_start;

      // done: the start and end are eqal to some value
      if(row_end == RENDER_COMPLETE && row_iter == RENDER_COMPLETE)
        render_complete = true;


      requestor_complete = true;
      get_work_var.notify_all();
      // note: all threads will wake up before notify_all returns, so this is safe
    }
  else
    {
      // wait on requestor_complete
      std::unique_lock<std::mutex> ul(wait_mutex);
      threads_seen++;
      get_work_var.wait(ul, [&] {return requestor_complete.load();});
      int ts = threads_seen--;
      if(ts == 1)
        requestor_complete = false;
    }

  if(was_requesting_thread)
    {
      int offset_pixels = (image_height - last_row_start) * image_width;
      int num_pixels = (last_row_start - last_row_end) * image_width;

      MPI_Put(output_image + offset_pixels,
              num_pixels,
              color_type, 0,
              offset_pixels,
              num_pixels,
              color_type, window
              );

      was_requesting_thread = false;
    }


  goto RENDER_LOOP;
}


void raytracing(const traceConfig config, int num_threads){
    const camera &cam = config.cam;
    const int image_width = config.width;
    const int image_height = config.height;
    const int max_depth = config.traceDepth;
    const int samples_per_pixel = config.samplePerPixel;
    const int rows_per_process = image_height / config.numProcs;

    if(config.myRank == 0)
      {
        std ::cout << "P3\n"
                   << image_width << ' ' << image_height << "\n255\n";
      }

  MPI_Win window;
  MPI_Datatype MPI_COLOR;
  color *output_image = nullptr;
  MPI_Alloc_mem(sizeof(color) * image_height * image_width,
                MPI_INFO_NULL,
                &output_image
                );
  for(int i = 0; i < image_height * image_width; i++)
    {
      output_image[i] = color(255, 0, 255);
    }

  MPI_Win_create(output_image,
                 sizeof(color) * image_height * image_width,
                 sizeof(color),
                 MPI_INFO_NULL, MPI_COMM_WORLD,
                 &window);

  MPI_Type_contiguous(3, MPI_DOUBLE, &MPI_COLOR);
  MPI_Type_commit(&MPI_COLOR);


  double tstart = omp_get_wtime();
  MPI_Win_fence(0, window);
  std::atomic_int remaining_iters{-1};
  std::atomic_int row_end{-1};

  // TODO: change this
  // TODO: add thread_config as analog to traceConfig
  int minimum_distribution = num_threads;
  if(config.myRank == 0)
    {
      std::thread work_distributor = std::thread(work_distributor_loop,
                                                 std::ref(config),
                                                 minimum_distribution
                                                 );
      work_distributor.join();
    }
  else
    {
          std::atomic<bool> requestor_complete{false};
          std::atomic<bool> render_complete{false};
          std::condition_variable work_var;
          std::mutex wait_mutex;
          std::atomic_int prev_row_start{0};
          std::atomic_int prev_row_end{0};
          std::atomic_int threads_seen{0};

          std::thread threads[num_threads];
          for(int i = 0; i < num_threads; i++)
            {
              threads[i] = std::thread(render_loop,
                                       std::ref(config),
                                       output_image,
                                       std::ref(remaining_iters),
                                       std::ref(row_end),
                                       std::ref(requestor_complete),
                                       std::ref(render_complete),
                                       std::ref(work_var),
                                       std::ref(wait_mutex),
                                       std::ref(prev_row_start),
                                       std::ref(prev_row_end),
                                       std::ref(threads_seen),
                                       num_threads,
                                       std::ref(MPI_COLOR),
                                       std::ref(window)
                                       );
            }
          for(int i = 0; i < num_threads; i++)
            {
              threads[i].join();
            }

    }
  double tend = omp_get_wtime();
  MPI_Win_fence(0, window);
  double t_elapsed = tend - tstart;
  double tend_all = omp_get_wtime();

  double *receive_data = nullptr;

  if(config.myRank == 0)
    {
      receive_data = new double[config.numProcs];
    }

  MPI_Gather(&t_elapsed,
             1,
             MPI_DOUBLE,
             receive_data,
             1,
             MPI_DOUBLE,
             0,
             MPI_COMM_WORLD
             );

  if(config.myRank == 0)
    {
      for(int i = 0; i < image_height * image_width; i++)
        write_color(std::cout, output_image[i], samples_per_pixel);
  }

  if(config.myRank == 0)
    {
      std::cerr << "TIME_ALL: " << tend_all - tstart << "\n";

      for(int i = 0; i < config.numProcs; i++)
        {
          std::cerr << "TIME_PROCESS: " << i << " " << receive_data[i] << "\n";
        }
    }

  MPI_Win_free(&window);
  MPI_Free_mem(output_image);
  MPI_Type_free(&MPI_COLOR);

  delete[] receive_data;
}



int main(int argc, char **argv)
{

  int multithread_support = 0;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &multithread_support);

  assert(multithread_support == MPI_THREAD_FUNNELED);

  if(argc<3){
    std::cerr<<"Usage:"<<argv[0]<<" sceneFile num_threads"<<std::endl;
    exit(1);
  }

  int my_rank = 0;
  int nprocs = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  std::string sceneFile = argv[1];
  ShapeDataIO io;
  std::vector<Sphere*> scene_spheres = io.load_scene(sceneFile);
  int num_threads = std::atoi(argv[2]);
  omp_set_num_threads(num_threads);

  if(my_rank == 0)
    {
      std::cerr << "Rendering scene " << sceneFile
                << " using " << num_threads
                << " threads per process, " << nprocs << " processes.\n";
    }

    // Image
    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 500;
    const int max_depth = 50;

  // World
  BVH world(scene_spheres);

  point3 lookfrom(13, 2, 3);
  point3 lookat(0, 0, 0);
  vec3 vup(0, 1, 0);
  auto dist_to_focus = 10.0;
  auto aperture = 0.1;
  camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

  traceConfig config(cam, world, image_width, image_height, max_depth, samples_per_pixel, nprocs, my_rank, num_threads);

  raytracing(config, num_threads);
  if(my_rank == 0)
    {
      std::cerr << "\nDone.\n";
    }

  io.clear_scene(scene_spheres);
  MPI_Finalize();
}
