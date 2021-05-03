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

  if(argc<4){
    std::cerr<<"Usage:"<<argv[0]<<" sceneFile num_threads tileSize"<<std::endl;
    exit(1);
  }

  ShapeDataIO shapeIO;
  std::string sceneFile = argv[1];
  std::vector<Sphere*> scene_spheres = shapeIO.load_scene(sceneFile);
  int num_threads = std::atoi(argv[2]);
  int tileSize = std::atoi(argv[3]);
  std::cerr << "Rendering scene " << sceneFile << " using " << num_threads << " threads with tilesize "<<tileSize<<std::endl;

    camera cam = camera::getDefault();
    // Image
    const int image_width = 640;
    const int image_height = static_cast<int>(image_width / cam.aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 10;

    // World
    BVH world(scene_spheres);
    traceConfig config(cam, image_width, image_height, max_depth, samples_per_pixel, num_threads, 0, 1, true);
    raytracing_bvh_tiled(config, world,tileSize);
    std::cerr << "\nDone.\n";
    shapeIO.clear_scene(scene_spheres);
}