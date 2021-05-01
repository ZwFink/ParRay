#include <benchmark/benchmark.h>
#include "ray_tracing.h"
#include "data_porting.h"
#include "vec3.h"
#include "camera.h"


static void BM_BVH_Tracing(benchmark::State &state)
{
    ShapeDataIO io;
    std::vector<Sphere *> sceneSpheres = io.load_scene("./random_spheres_scene.data");
    // Image
    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 100;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 3;
    BVH world(sceneSpheres);
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;
    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    int num_threads = state.range(0);
    traceConfig config(cam, world, image_width, image_height, max_depth, samples_per_pixel, num_threads,0,1);

    for (auto _ : state){
     raytracing_bvh(config);
    }
    
    io.clear_scene(sceneSpheres);
}
BENCHMARK(BM_BVH_Tracing)->RangeMultiplier(2)->Range(1,32);

BENCHMARK_MAIN();