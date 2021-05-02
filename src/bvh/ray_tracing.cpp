#include "ray_tracing.h"
#include "common.h"
#include <omp.h>
#include "bvh.hpp"
#include <fstream>
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

long hitRate = 0;
long missRate[5];
long maxDeptCount = 0;
std::vector<ray> hitRayBuffer;
std::vector<ray> missRayBuffer;

struct debugInfo
{
    const ray r;
    bool bvhHit;
    debugInfo(const ray &_r, bool _bvhHit):r(_r),bvhHit(_bvhHit){};
};
std::vector<debugInfo> differentRay;

void printRay(std::ostream &os, const ray &ray, bool isHit)
{
    os.precision(8);
    os << "vec3(" << ray.orig.x() << "," << ray.orig.y() << "," << ray.orig.z()<<")";
    os << "vec3(" << ray.dir.x() << "," << ray.dir.y() << "," << ray.dir.z()<<")";
    os << " isHit=" << isHit;
    os << std::endl;
}
void printRays(std::vector<ray> &hitRays, std::vector<ray> &missRays)
{
    std::string fileName = "bvh-ray.log";
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

void printDiffRays(std::vector<debugInfo> &diffRays){
    std::string fileName = "diff-ray.log";
    std::ofstream file;
    file.open(fileName);
    for (auto &e : diffRays)
    {
        printRay(file, e.r, e.bvhHit);
    }
}

color ray_color(const ray &r, BVH &world, hittable_list &simpleWorld, int depth){
    hit_record rec;
    Sphere *hitObject = nullptr;

    if (depth <= 0)
    {
        maxDeptCount++;
        return color(255, 0, 255);
    }

    bool bvh_hit = world.intersect(r, &hitObject, rec);
    bool simple_hit = simpleWorld.hit(r, 0.001, DBL_MAX, rec);
    if(bvh_hit!=simple_hit && differentRay.size()<200){
        differentRay.push_back(debugInfo(r,bvh_hit));
    }

    if (bvh_hit)
    {
        ray scattered;
        color attenuation;

        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        {
            return ray_color(scattered, world, simpleWorld, depth - 1);
        }
        else
        {
            return color(255);
        }
    }

    missRate[depth]++;
    if (depth == 1)
    {
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
}

color ray_color(const ray &r, BVH &world, int depth)
{
    hit_record rec;
    Sphere *hitObject = nullptr;

    if (depth <= 0)
    {
        maxDeptCount++;
        return color(255, 0, 255);
    }

    if (world.intersect(r, &hitObject, rec))
    {
        ray scattered;
        color attenuation;

        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        {
            return ray_color(scattered, world, depth - 1);
        }
        else
        {
            return color(255);
        }
    }

    missRate[depth]++;
    if (depth == 1)
    {
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

    //otherwise return the background color
    /*
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
    */
}

void raytracing_bvh_single_threaded_debug(const traceConfig &config)
{
    const camera &cam = config.cam;
    const int image_width = config.width;
    const int image_height = config.height;
    const int max_depth = config.traceDepth;
    const int samples_per_pixel = config.samplePerPixel;
    BVH &world = config.world;
    const int threadNumer = config.numProcs;

    hittable_list simpleworld;
    auto ground_material = make_unique<dielectric>(1.5);
    simpleworld.add(make_unique<sphere>(point3(-1000, 1, 1), 1000, std::move(ground_material)));

    color *out_image = new color[image_width * image_height];

    {
        for (int j = config.height - 1; j >= 0; j--)
        {
            for (int i = 0; i < config.width; i++)
            {
                color pixel_color(0, 0, 0);
                for (int s = 0; s < samples_per_pixel; ++s)
                {
                    auto u = (i + random_double()) / (image_width - 1);
                    auto v = (j + random_double()) / (image_height - 1);

                    ray r = cam.get_ray(u, v);
                    pixel_color += ray_color(r, world, simpleworld, max_depth);
                }
                out_image[((image_height - 1 - j) * image_width + i)] = pixel_color;
            }
        }
    }
    std ::cout << "P3\n"
               << image_width << ' ' << image_height << "\n255\n";
    for (int i = 0; i < image_height * image_width; i++)
        write_color(std::cout, out_image[i], samples_per_pixel);
    std::cerr << "\nDone.\n miss rate 2-bvh maxDepthCount: " << maxDeptCount << std::endl;
    for (int i = 1; i <= 3; i++)
    {
        std::cerr << "Depth " << i - 1 << " miss rate" << missRate[i - 1] << std::endl;
    }

    printDiffRays(differentRay);

    delete[] out_image;
}



void raytracing_bvh_single_threaded(const traceConfig &config)
{
    const camera &cam = config.cam;
    const int image_width = config.width;
    const int image_height = config.height;
    const int max_depth = config.traceDepth;
    const int samples_per_pixel = config.samplePerPixel;
    BVH &world = config.world;
    const int threadNumer = config.numProcs;

    color *out_image = new color[image_width * image_height];

    {
        for (int j = config.height - 1; j >= 0; j--)
        {
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
    std ::cout << "P3\n"
               << image_width << ' ' << image_height << "\n255\n";
    for (int i = 0; i < image_height * image_width; i++)
        write_color(std::cout, out_image[i], samples_per_pixel);
    std::cerr << "\nDone.\n miss rate 2-bvh maxDepthCount: " << maxDeptCount << std::endl;
    for (int i = 1; i <= 5; i++)
    {
        std::cerr << "Depth " << i - 1 << " miss rate" << missRate[i - 1] << std::endl;
    }
    delete[] out_image;
}

void raytracing_bvh(const traceConfig &config)
{
    const camera &cam = config.cam;
    const int image_width = config.width;
    const int image_height = config.height;
    const int max_depth = config.traceDepth;
    const int samples_per_pixel = config.samplePerPixel;
    BVH &world = config.world;
    const int threadNumer = config.numProcs;

    color *out_image = new color[image_width * image_height];

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

    /*   std ::cout << "P3\n"
               << image_width << ' ' << image_height << "\n255\n";
    for (int i = 0; i < image_height * image_width; i++)
        write_color(std::cout, out_image[i], samples_per_pixel);
    std::cerr << "\n\nElapsed time: " << tend - tstart << "\n";
    std::cerr << "\nDone.\n";*/
    delete[] out_image;
}
