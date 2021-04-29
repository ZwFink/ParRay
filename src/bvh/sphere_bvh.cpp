#include "bvh.hpp"
#include "data_porting.h"
#include "vec3.h"
#include "camera.h"
#include <vector>
#include "color.h"
#include <ctime>
#include "boundable.h"

BVH random_scene()
{

    std::vector<std::shared_ptr<Sphere>> sceneObjects;
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    //add ground
    sceneObjects.emplace_back(new Sphere(point3(0, -1000, 0), 1000, ground_material));

    auto albedo = color::random(0.5, 1);
    auto fuzz = random_double(0, 0.5);
    auto metal_material = make_shared<metal>(albedo, fuzz);

    sceneObjects.emplace_back(new Sphere(point3(0, 2, 1.5), 1, metal_material));
    BVH world(sceneObjects);
    return world;
}

BVH static_scene()
{
    std::vector<std::shared_ptr<Sphere>> sceneObjects;
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto glass_material = make_shared<dielectric>(1.5);

    //metal
    auto albedo = color::random(0.5, 1);
    auto fuzz = random_double(0, 0.5);
    auto metal_material = make_shared<metal>(albedo, fuzz);
    sceneObjects.emplace_back(new Sphere(point3(0, -1000, 0), 1000, ground_material));
    sceneObjects.emplace_back(new Sphere(point3(5, 1.5, 1), 1, glass_material));
    sceneObjects.emplace_back(new Sphere(point3(1, 1, 1), 1, metal_material));
    BVH world(sceneObjects);
    return world;
}



color ray_color(const ray &r, BVH &world, int depth)
{
    hit_record rec;
    std::shared_ptr<Sphere> hitObject(nullptr);

    if (depth <= 0)
        return color(0, 0, 0);

    if (world.intersect(r, hitObject, rec))
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

std::vector<std::shared_ptr<Sphere>> load_scene(std::string fileName){
  ShapeDataIO io;
  std::cout<<"Loading "<<fileName<<std::endl;
  nlohmann::json j = io.read(fileName);
  return io.deserialize_Spheres(j);
}

int main(int argc, char** argv)
{

  if(argc<2){
    std::cerr<<"Usage:"<<argv[0]<<" sceneFile"<<std::endl;
    exit(1);
  }

  std::string sceneFile = argv[1];
  std::vector<std::shared_ptr<Sphere>> scene_spheres = load_scene(sceneFile);

    // Image
    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 1000;
    const int max_depth = 10;

  // World
  BVH world(scene_spheres);

  point3 lookfrom(13, 2, 3);
  point3 lookat(0, 0, 0);
  vec3 vup(0, 1, 0);
  auto dist_to_focus = 10.0;
  auto aperture = 0.1;
  camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

  std :: cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
  std::clock_t c_start = std::clock();
  for(int j = image_height - 1; j >= 0; j--)
    {
      std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
      for(int i = 0; i < image_width; i++)
        {
          color pixel_color(0, 0, 0);
          for(int s = 0; s < samples_per_pixel; ++s)
            {
              auto u = (i + random_double()) / (image_width - 1);
              auto v = (j + random_double()) / (image_height - 1);

              ray r = cam.get_ray(u, v);
              pixel_color += ray_color(r, world, max_depth);
            }
          write_color(std::cout, pixel_color, samples_per_pixel);
        }

    }
  std::clock_t c_end = std::clock();
  std::cerr << "\nDone.\n";
  std::cerr << "\nTotal time: "<<(c_end-c_start)/CLOCKS_PER_SEC<<std::endl;
}
