#include "sphere_generation.h"

std::vector<sphere*> SphereGeneration::random_scene_spheres(int size)
{
    std::vector<sphere*> output;

  auto ground_material = make_unique<lambertian>(color(0.5, 0.5, 0.5));
  output.push_back(new sphere(point3(0, -1000, 0), 1000, std::move(ground_material)));

  for (int a = -1*size; a < size; a++)
  {
    for (int b = -1*size; b < size; b++)
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
          output.push_back(new sphere(center, 0.2, std::move(sphere_material)));
        }
        else if (choose_mat < 0.95)
        {
          // metal
          auto albedo = color::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = make_unique<metal>(albedo, fuzz);
          output.push_back(new sphere(center, 0.2, std::move(sphere_material)));
        }
        else
        {
          // glass
          sphere_material = make_unique<dielectric>(1.5);
          output.push_back(new sphere(center, 0.2, std::move(sphere_material)));
        }
      }
    }
  }

  auto material1 = make_unique<dielectric>(1.5);
  output.push_back(new sphere(point3(0, 1, 0), 1.0, std::move(material1)));

  auto material2 = make_unique<lambertian>(color(0.4, 0.2, 0.1));
  output.push_back(new sphere(point3(-4, 1, 0), 1.0, std::move(material2)));

  auto material3 = make_unique<metal>(color(0.7, 0.6, 0.5), 0.0);
  output.push_back(new sphere(point3(4, 1, 0), 1.0, std::move(material3)));

  return output;
}

std::vector<Sphere*> SphereGeneration::random_scene_Spheres(int size)
{
    std::vector<Sphere*> output;

  auto ground_material = new lambertian(color(0.5, 0.5, 0.5));
  output.push_back(new Sphere(point3(0, -1000, 0), 1000, ground_material));

  for (int a = -1*size; a < size; a++)
  {
    for (int b = -1*size; b < size; b++)
    {
      auto choose_mat = random_double();
      point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - point3(4, 0.2, 0)).length() > 0.9)
      {
        material* sphere_material;

        if (choose_mat < 0.8)
        {
          // diffuse
          auto albedo = color::random() * color::random();
          sphere_material = new lambertian(albedo);
          output.push_back(new Sphere(center, 0.2, sphere_material));
        }
        else if (choose_mat < 0.95)
        {
          // metal
          auto albedo = color::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = new metal(albedo, fuzz);
          output.push_back(new Sphere(center, 0.2, sphere_material));
        }
        else
        {
          // glass
          sphere_material = new dielectric(1.5);
          output.push_back(new Sphere(center, 0.2, sphere_material));
        }
      }
    }
  }

  auto material1 = new dielectric(1.5);
  output.push_back(new Sphere(point3(0, 1, 0), 1.0, material1));

  auto material2 = new lambertian(color(0.4, 0.2, 0.1));
  output.push_back(new Sphere(point3(-4, 1, 0), 1.0, material2));

  auto material3 = new metal(color(0.7, 0.6, 0.5), 0.0);
  output.push_back(new Sphere(point3(4, 1, 0), 1.0, material3));

  return output;
}



