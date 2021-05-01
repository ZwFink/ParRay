#include "ray_tracing.h"
#include "common.h"
#include <omp.h>
#include "bvh.hpp"

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

void raytracing_bvh(const traceConfig &config)
{
    const camera &cam = config.cam;
    const int image_width = config.width;
    const int image_height = config.height;
    const int max_depth = config.traceDepth;
    const int samples_per_pixel = config.samplePerPixel;
    BVH &world = config.world;
    color *out_image = config.out_image;
    const int threadNumer = config.threadNumber;

    std ::cout << "P3\n"
               << image_width << ' ' << image_height << "\n255\n";

    omp_set_num_threads(threadNumer);
    double tstart = omp_get_wtime();
#pragma omp parallel shared(out_image, cam)
    {
#pragma omp for schedule(dynamic)
        for (int j = config.height - 1; j >= 0; j--)
        {
            // std::cerr << "\rScanlines remaining: " << j << ' ' << omp_get_thread_num() << std::endl;

            for (int i = 0; i < config.width; i++)
            {
                color pixel_color(0, 0, 0);
                for (int s = 0; s < samples_per_pixel; ++s)
                {
                    auto u = (i + random_double()) / (image_width - 1);
                    auto v = (j + random_double()) / (image_height - 1);

                    ray r = cam.get_ray(u, v);
                    pixel_color += ray_color(r, world, max_depth);
                }
                out_image[((image_height - 1 - j) * image_width + i)] = pixel_color;
            }
        }
    }

    double tend = omp_get_wtime();
    for (int i = 0; i < image_height * image_width; i++)
        write_color(std::cout, out_image[i], samples_per_pixel);
    std::cerr << "\n\nElapsed time: " << tend - tstart << "\n";
    std::cerr << "\nDone.\n";
}
