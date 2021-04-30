#include "data_porting.h"
#include "hittable_list.h"
#include "hittable.h"
#include "material.h"
#include "sphere.h"
#include "common.h"
#include <nlohmann/json.hpp>

std::vector<shared_ptr<sphere>> random_scene(int size)
{
    std::vector<std::shared_ptr<sphere>> output;

  auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
  output.push_back(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

  for (int a = -1*size; a < size; a++)
  {
    for (int b = -1*size; b < size; b++)
    {
      auto choose_mat = random_double();
      point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - point3(4, 0.2, 0)).length() > 0.9)
      {
        shared_ptr<material> sphere_material;

        if (choose_mat < 0.8)
        {
          // diffuse
          auto albedo = color::random() * color::random();
          sphere_material = make_shared<lambertian>(albedo);
          output.push_back(make_shared<sphere>(center, 0.2, sphere_material));
        }
        else if (choose_mat < 0.95)
        {
          // metal
          auto albedo = color::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = make_shared<metal>(albedo, fuzz);
          output.push_back(make_shared<sphere>(center, 0.2, sphere_material));
        }
        else
        {
          // glass
          sphere_material = make_shared<dielectric>(1.5);
          output.push_back(make_shared<sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  auto material1 = make_shared<dielectric>(1.5);
  output.push_back(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

  auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
  output.push_back(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

  auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
  output.push_back(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

  return output;
}

int main(int argc, char **argv){
    if(argc<2){
      std::cerr<<"Usage: "<<argv[0]<<" size"<<std::endl;
      exit(1);
    }
    int size = atoi(argv[1]);
    ShapeDataIO io;
    auto spheres = random_scene(size);
    nlohmann::json spheresJson = io.serialize(spheres);
    io.write("random_spheres_scene.data", spheresJson);
    auto readJson = io.read("random_spheres_scene.data");
    auto readSpheres = io.deserialize_Spheres(readJson);

    std::cout<<readSpheres.size()<<" records are generated."<<std::endl;
    return 0;
}