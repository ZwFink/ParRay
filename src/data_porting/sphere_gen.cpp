#include "data_porting.h"
#include "hittable_list.h"
#include "hittable.h"
#include "material.h"
#include "sphere.h"
#include "common.h"
#include <nlohmann/json.hpp>
#include "sphere_generation.h"

int main(int argc, char **argv){
    if(argc<2){
      std::cerr<<"Usage: "<<argv[0]<<" size"<<std::endl;
      exit(1);
    }
    int size = atoi(argv[1]);
    ShapeDataIO io;
    SphereGeneration sg;
    auto spheres = sg.random_scene_spheres(size);
    nlohmann::json spheresJson = io.serialize(spheres);
    io.write("random_spheres_scene.data", spheresJson);
    auto readJson = io.read("random_spheres_scene.data");
    auto readSpheres = io.deserialize_Spheres(readJson);
    std::cout<<readSpheres.size()<<" records are generated."<<std::endl;
    return 0;
}