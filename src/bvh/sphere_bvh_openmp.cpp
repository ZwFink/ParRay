#include "bvh.hpp"
#include "ray_tracing.h"
#include "data_porting.h"
#include "vec3.h"
#include "camera.h"
#include <vector>
#include "color.h"
#include <ctime>
#include "boundable.h"


std::vector<Sphere*> load_scene(std::string fileName){
  ShapeDataIO io;
  std::cerr<<"Loading "<<fileName<<std::endl;
  nlohmann::json j = io.read(fileName);
  return io.deserialize_Spheres(j);
}

void clear_scene(std::vector<Sphere*> &input){
  for(int i=0; i<input.size();i++){
    delete input[i];
    input[i]=nullptr;
  }
}

int main(int argc, char** argv)
{

  if(argc<3){
    std::cerr<<"Usage:"<<argv[0]<<" sceneFile num_threads"<<std::endl;
    exit(1);
  }

  std::string sceneFile = argv[1];
  std::vector<Sphere*> scene_spheres = load_scene(sceneFile);
  int num_threads = std::atoi(argv[2]);
  std::cerr << "Rendering scene " << sceneFile << " using " << num_threads << " threads\n";

    // Image
    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 100;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 1;
    const int max_depth = 1;

  // World
  BVH world(scene_spheres);

  point3 lookfrom(13, 2, 3);
  point3 lookat(0, 0, 0);
  vec3 vup(0, 1, 0);
  auto dist_to_focus = 10.0;
  auto aperture = 0.1;
  camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

  traceConfig config(cam, world, image_width, image_height, max_depth, samples_per_pixel, num_threads);

  raytracing_bvh(config);
  std::cerr << "\nDone.\n";
  clear_scene(scene_spheres);
}
