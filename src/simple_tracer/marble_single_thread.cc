#include <iostream>
#include <omp.h>
#include "common.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include <cassert>
#include "sphere_generation.h"
#include "ray_tracing.h"

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

int main()
{

  SphereGeneration scene_generator;
  // World
  auto world = scene_generator.random_scene_hittablelist(11);
  camera cam = camera::getDefault();

  //trace property
  const int image_width = 1200;
  const int image_height = static_cast<int>(image_width / cam.aspect_ratio);
  const int samples_per_pixel = 500;
  const int max_depth = 10;

  color *output_image = new color[image_height * image_width];
  traceConfig config(cam, image_width, image_height, max_depth, samples_per_pixel,1,0,1);


  std ::cout << "P3\n"
             << image_width << ' ' << image_height << "\n255\n";

  double tstart = omp_get_wtime();
  raytracing_hittablelist(config, world);
  double tend = omp_get_wtime();

  for (int i = 0; i < image_height * image_width; i++)
    write_color(std::cout, output_image[i], samples_per_pixel);

  std::cerr << "\n\nElapsed time: " << tend - tstart << "\n";
  std::cerr << "\nDone.\n";
}
