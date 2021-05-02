#include <iostream>
#include <omp.h>
#include "common.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include <cassert>
#include <fstream>

long hitRate = 0;
long missRate[3];
long maxDeptCount = 0;
std::vector<ray> hitRayBuffer;
std::vector<ray> missRayBuffer;

void printRay(std::ostream &os, const ray &ray, bool isHit)
{
  os << "origin x: " << ray.orig.x() << " y: " << ray.orig.y() << " z: " << ray.orig.z();
  os << " dir x: " << ray.dir.x() << " y: " << ray.dir.y() << " z: " << ray.dir.z();
  os << " isHit=" << isHit;
  os << std::endl;
}
void printRays(std::vector<ray> &hitRays, std::vector<ray> &missRays)
{
  std::string fileName = "simple-ray.log";
  std::ofstream file;
  file.open(fileName);
  for (auto &e : hitRays)
  {
    printRay(file, e, true);
  }
  for (auto &e : missRays)
  {
    printRay(file, e, false);
  }
}

hittable_list static_scene()
{
  hittable_list world;
  auto ground_material = make_unique<lambertian>(color(0.5, 0.5, 0.5));
  auto glass_material = make_unique<dielectric>(1.5);

  //metal
  auto albedo = color::random(0.5, 1);
  auto fuzz = random_double(0, 0.5);
  auto metal_material = make_unique<metal>(albedo, fuzz);

  world.add(make_unique<sphere>(point3(0, -1000, 0), 1000, std::move(ground_material)));
  world.add(make_unique<sphere>(point3(5, 1.5, 1), 1, std::move(glass_material)));
  world.add(make_unique<sphere>(point3(1, 1, 1), 1, std::move(metal_material)));
  return world;
}

hittable_list static_scene_1_dieletric()
{
  hittable_list world;
  auto material1 = make_unique<dielectric>(1.5);
  world.add(make_unique<sphere>(point3(-1000, 1, 1), 1000, std::move(material1)));
  return world;
}

hittable_list random_scene()
{
  hittable_list world;

  auto ground_material = make_unique<lambertian>(color(0.5, 0.5, 0.5));
  world.add(make_unique<sphere>(point3(0, -1000, 0), 1000, std::move(ground_material)));

  for (int a = -11; a < 11; a++)
  {
    for (int b = -11; b < 11; b++)
    {
      auto choose_mat = random_double();
      point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - point3(4, 0.2, 0)).length() > 0.9)
      {
        unique_ptr<material> sphere_material;

        if (choose_mat < 0.8)
        {
          // diffuse
          auto albedo = color::random() * color::random();
          sphere_material = make_unique<lambertian>(albedo);
          world.add(make_unique<sphere>(center, 0.2, std::move(sphere_material)));
        }
        else if (choose_mat < 0.95)
        {
          // metal
          auto albedo = color::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = make_unique<metal>(albedo, fuzz);
          world.add(make_unique<sphere>(center, 0.2, std::move(sphere_material)));
        }
        else
        {
          // glass
          sphere_material = make_unique<dielectric>(1.5);
          world.add(make_unique<sphere>(center, 0.2, std::move(sphere_material)));
        }
      }
    }
  }
  auto material1 = make_unique<dielectric>(1.5);
  world.add(make_unique<sphere>(point3(0, 1, 0), 1.0, std::move(material1)));

  auto material2 = make_unique<lambertian>(color(0.4, 0.2, 0.1));
  world.add(make_unique<sphere>(point3(-4, 1, 0), 1.0, std::move(material2)));

  auto material3 = make_unique<metal>(color(0.7, 0.6, 0.5), 0.0);
  world.add(make_unique<sphere>(point3(4, 1, 0), 1.0, std::move(material3)));

  return world;
}

color ray_color(const ray &r, const hittable &world, int depth)
{
  hit_record rec;

  if (depth <= 0)
  {
    maxDeptCount++;
    return color(255, 0, 255);
  }

  if (world.hit(r, 0.001, infinity, rec))
  {
    ray scattered;
    color attenuation;

    if (depth == 1 && hitRayBuffer.size() < 100)
    {
      hitRayBuffer.push_back(r);
    }

    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
    {

      return ray_color(scattered, world, depth - 1);
      // return attenuation * ray_color(scattered, world, depth - 1);
    }
    else
    {
      return color(255);
    }
  }
  missRate[depth]++;
  if (depth == 1)
  {
    if (missRayBuffer.size() < 100)
      missRayBuffer.push_back(r);
    return color(0, 0, 255);
  }
  else if (depth == 2)
  {
    return color(0);
  }
  else
  {
    return color(255);
  }

  /*
  vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);

  return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
  */
}

int main()
{

  // Image
  const auto aspect_ratio = 3.0 / 2.0;
  const int image_width = 1200;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 10;
  const int max_depth = 2;

  // World
  auto world = static_scene_1_dieletric();

  point3 lookfrom(13, 0, 0);
  point3 lookat(0, 0, 0);
  vec3 vup(0, 1, 0);
  auto dist_to_focus = 10.0;
  auto aperture = 0.1;
  camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

  std ::cout << "P3\n"
             << image_width << ' ' << image_height << "\n255\n";

  color *output_image = new color[image_height * image_width];

  {
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
  for (int i = 0; i < image_height * image_width; i++)
    write_color(std::cout, output_image[i], samples_per_pixel);

  std::cerr << "\nDone.\n miss rate 1-simple maxDepthCount: " << maxDeptCount << std::endl;
  for (int i = 1; i <= 3; i++)
  {
    std::cerr << "Depth " << i - 1 << " miss rate" << missRate[i - 1] << std::endl;
  }
  printRays(hitRayBuffer, missRayBuffer);
}
