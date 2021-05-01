#include "bvh.hpp"
// #include "data_porting.h"
#include "vec3.h"
#include "camera.h"
#include <vector>
#include "color.h"
#include <ctime>
#include "boundable.h"
#include <omp.h>

BVH random_scene()
{

  std::vector<Sphere *> sceneObjects;
  auto ground_material = new lambertian(color(0.5, 0.5, 0.5));
  //add ground
  sceneObjects.push_back(new Sphere(point3(0, -1000, 0), 1000, dynamic_cast<material *>(ground_material)));

  auto albedo = color::random(0.5, 1);
  auto fuzz = random_double(0, 0.5);
  auto metal_material = new metal(albedo, fuzz);

  sceneObjects.push_back(new Sphere(point3(0, 2, 1.5), 1, metal_material));
  BVH world(sceneObjects);
  return world;
}

BVH static_scene()
{
  std::vector<Sphere *> sceneObjects;
  auto ground_material = new lambertian(color(0.5, 0.5, 0.5));
  auto glass_material = new dielectric(1.5);

  //metal
  auto albedo = color::random(0.5, 1);
  auto fuzz = random_double(0, 0.5);
  auto metal_material = new metal(albedo, fuzz);
  sceneObjects.push_back(new Sphere(point3(0, -1000, 0), 1000, dynamic_cast<material *>(ground_material)));
  sceneObjects.push_back(new Sphere(point3(5, 1.5, 1), 1, dynamic_cast<material *>(glass_material)));
  sceneObjects.push_back(new Sphere(point3(1, 1, 1), 1, dynamic_cast<material *>(metal_material)));
  std::cout << "World created" << std::endl;
  return BVH(sceneObjects);
}

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

// std::vector<std::shared_ptr<Sphere>> load_scene(std::string fileName){
//   ShapeDataIO io;
//   std::cout<<"Loading "<<fileName<<std::endl;
//   nlohmann::json j = io.read(fileName);
//   return io.deserialize_Spheres(j);
// }

int main(int argc, char **argv)
{

  // if(argc<2){
  //   std::cerr<<"Usage:"<<argv[0]<<" sceneFile"<<std::endl;
  //   exit(1);
  // }

  // std::string sceneFile = argv[1];
  // std::vector<std::shared_ptr<Sphere>> scene_spheres = load_scene(sceneFile);

  // Image
  const auto aspect_ratio = 3.0 / 2.0;
  const int image_width = 100;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 1;
  const int max_depth = 1;

  // World
  // BVH world(scene_spheres);

  std::vector<Sphere *> sceneObjects;

  auto ground_material = new lambertian(color(0.5, 0.5, 0.5));
  sceneObjects.push_back(new Sphere(point3(0, -1000, 0), 1000, dynamic_cast<material *>(ground_material)));

  for (int a = -11; a < 11; a++)
  {
    for (int b = -11; b < 11; b++)
    {
      auto choose_mat = random_double();
      point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - point3(4, 0.2, 0)).length() > 0.9)
      {
        material *sphere_material;

        if (choose_mat < 0.8)
        {
          // diffuse
          auto albedo = color::random() * color::random();
          sphere_material = new lambertian(albedo);
          sceneObjects.push_back(new Sphere(center, 0.2, dynamic_cast<material *>(sphere_material)));
        }
        else if (choose_mat < 0.95)
        {
          // metal
          auto albedo = color::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = new metal(albedo, fuzz);
          sceneObjects.push_back(new Sphere(center, 0.2, dynamic_cast<material *>(sphere_material)));
        }
        else
        {
          // glass
          sphere_material = new dielectric(1.5);
          sceneObjects.push_back(new Sphere(center, 0.2, dynamic_cast<material *>(sphere_material)));
        }
      }
    }
  }
  auto material1 = new dielectric(1.5);
  sceneObjects.push_back(new Sphere(point3(0, 1, 0), 1.0, dynamic_cast<material *>(material1)));

  auto material2 = new lambertian(color(0.4, 0.2, 0.1));
  sceneObjects.push_back(new Sphere(point3(-4, 1, 0), 1.0, dynamic_cast<material *>(material2)));

  auto material3 = new metal(color(0.7, 0.6, 0.5), 0.0);
  sceneObjects.push_back(new Sphere(point3(4, 1, 0), 1.0, dynamic_cast<material *>(material3)));

  BVH world(sceneObjects);

  point3 lookfrom(13, 2, 3);
  point3 lookat(0, 0, 0);
  vec3 vup(0, 1, 0);
  auto dist_to_focus = 10.0;
  auto aperture = 0.1;
  camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

  std ::cout << "P3\n"
             << image_width << ' ' << image_height << "\n255\n";

  color *output_image = new color[image_height * image_width];
  double tstart = omp_get_wtime();

  omp_set_num_threads(4);
#pragma omp parallel shared(output_image, cam)
  {
#pragma omp for schedule(dynamic)
    for (int j = image_height - 1; j >= 0; j--)
    {
      // std::cerr << "\rScanlines remaining: " << j << ' ' << omp_get_thread_num() << std::endl;

      for (int i = 0; i < image_width; i++)
      {
        color pixel_color(0, 0, 0);
        for (int s = 0; s < samples_per_pixel; ++s)
        {
          auto u = (i + random_double()) / (image_width - 1);
          auto v = (j + random_double()) / (image_height - 1);

          ray r = cam.get_ray(u, v);
          pixel_color += ray_color(r, world, max_depth);
        }
        output_image[((image_height - 1 - j) * image_width + i)] = pixel_color;
      }
    }
  }

  double tend = omp_get_wtime();
  for (int i = 0; i < image_height * image_width; i++)
    write_color(std::cout, output_image[i], samples_per_pixel);
  std::cerr << "\n\nElapsed time: " << tend - tstart << "\n";
  std::cerr << "\nDone.\n";

  //clean up
  for(int i=0; i<sceneObjects.size();i++){
    if(sceneObjects[i]!=nullptr){
      delete sceneObjects[i];
      sceneObjects[i]=nullptr;
    }
  }
  if(output_image!=nullptr){
    delete[] output_image;
    output_image = nullptr;
  }
}
