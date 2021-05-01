#include "bvh.hpp"
#include "ray_tracing.h"
#include "data_porting.h"
#include "vec3.h"
#include "camera.h"
#include <vector>
#include "color.h"
#include <ctime>
#include "boundable.h"

int main(int argc, char** argv)
{

  if(argc<2){
    std::cerr<<"Usage:"<<argv[0]<<" sceneFile"<<std::endl;
    exit(1);
  }

  ShapeDataIO shapeIO;
  std::string sceneFile = argv[1];
  std::vector<Sphere*> scene_spheres = shapeIO.load_scene(sceneFile);

    // Image
    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 100;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 1;
    const int max_depth = 30;

  // World
  BVH world(scene_spheres);

  point3 lookfrom(13, 2, 3);
  point3 lookat(0, 0, 0);
  vec3 vup(0, 1, 0);
  auto dist_to_focus = 10.0;
  auto aperture = 0.1;
  camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

  traceConfig config(cam, world, image_width, image_height, max_depth, samples_per_pixel, 1, 0, 1);

  raytracing_bvh_single_threaded(config);

  shapeIO.clear_scene(scene_spheres);
}

