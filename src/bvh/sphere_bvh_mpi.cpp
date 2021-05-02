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
#include "ray_tracing.h"

color ray_color(const ray &r, BVH &world, int depth)
{
    hit_record rec;
    Sphere *hitObject = nullptr;

    if (depth <= 0)
        return color(0, 0, 0);

    if (world.intersect(r, &hitObject, rec))
    {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        {
            return attenuation * ray_color(scattered, world, depth - 1);
        }
        else
        {
            return color(0, 0, 0);
        }
    }

    //otherwise return the background color
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

void raytracing(const traceConfig config, BVH& world){
    const camera &cam = config.cam;
    const int image_width = config.width;
    const int image_height = config.height;
    const int max_depth = config.traceDepth;
    const int samples_per_pixel = config.samplePerPixel;
    const int rows_per_process = image_height / config.numProcs;
    int my_row_start = (1 + config.myRank) * rows_per_process;
    const int my_row_end = config.myRank * rows_per_process;

    if(config.myRank == config.numProcs - 1)
      my_row_start = image_height;

    std::cerr << "Process " << config.myRank
              << " rendering rows " << my_row_end
              << " to " << my_row_start << "\n";

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

  MPI_Win_create(output_image,
                 sizeof(color) * image_height * image_width,
                 sizeof(color),
                 MPI_INFO_NULL, MPI_COMM_WORLD,
                 &window);

  MPI_Type_contiguous(3, MPI_DOUBLE, &MPI_COLOR);
  MPI_Type_commit(&MPI_COLOR);


  double tstart = omp_get_wtime();
  MPI_Win_fence(0, window);

  #pragma omp parallel shared(output_image, cam)
  {
      #pragma omp for schedule(dynamic)
      for(int j = my_row_start - 1; j >= my_row_end; j--)
        {
          for(int i = 0; i < config.width; i++)
            {
              color pixel_color(0, 0, 0);
              for(int s = 0; s < samples_per_pixel; ++s)
                {
                  auto u = (i + random_double()) / (image_width - 1);
                  auto v = (j + random_double()) / (image_height - 1);

                  ray r = cam.get_ray(u, v);
                  pixel_color += ray_color(r, world, max_depth);
                }
              output_image[((image_height - 1 - j)*image_width + i)] = pixel_color;
            }
        }
    }

  int offset_pixels = (image_height - my_row_start) * image_width;
  int num_pixels = (my_row_start - my_row_end) * image_width;

  if(config.myRank != 0)
    {
       MPI_Put(output_image + offset_pixels,
               num_pixels,
               MPI_COLOR, 0,
               offset_pixels,
               num_pixels,
               MPI_COLOR, window
               );

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
      std::cerr << "Rendering scene " << sceneFile << " using " << num_threads << " threads\n";
    }

    // Image
    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 10;
    const int max_depth = 50;

  // World
  BVH world(scene_spheres);

  point3 lookfrom(13, 2, 3);
  point3 lookat(0, 0, 0);
  vec3 vup(0, 1, 0);
  auto dist_to_focus = 10.0;
  auto aperture = 0.1;
  camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

  traceConfig config(cam, image_width, image_height, max_depth, samples_per_pixel, nprocs, my_rank, num_threads);

  raytracing(config, world);
  if(my_rank == 0)
    {
      std::cerr << "\nDone.\n";
    }
  MPI_Finalize();

  io.clear_scene(scene_spheres);
}
