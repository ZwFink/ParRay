#include <benchmark/benchmark.h>
#include "ray_tracing.h"
#include "data_porting.h"
#include "vec3.h"
#include "camera.h"
#include "sphere_generation.h"

static void BM_Baseline_Simple_Tracing_at_sceneSize(benchmark::State &state)
{
    int size = state.range(0);

    camera cam = camera::getDefault();
    // Image
    const int image_width = 100;
    const int image_height = static_cast<int>(image_width / cam.aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 4;
    //create scene
    SphereGeneration sphereGen;
    hittable_list world = sphereGen.random_scene_hittablelist(size);
    //create config
    traceConfig config(cam, image_width, image_height, max_depth, samples_per_pixel, 1,0,1);

    for (auto _ : state){
     raytracing_hittablelist(config,world);
    }
}
BENCHMARK(BM_Baseline_Simple_Tracing_at_sceneSize)->Unit(benchmark::kMillisecond)->RangeMultiplier(2)->Range(1,10);



static void BM_BVH_Tracing_at_threadNum(benchmark::State &state)
{
    ShapeDataIO io;
    std::vector<Sphere *> sceneSpheres = io.load_scene("./random_spheres_scene.data");

    camera cam = camera::getDefault();
    // Image
    const int image_width = 100;
    const int image_height = static_cast<int>(image_width / cam.aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 3;
    BVH world(sceneSpheres);

    int num_threads = state.range(0);
    traceConfig config(cam, image_width, image_height, max_depth, samples_per_pixel, num_threads,0,1);

    for (auto _ : state){
     raytracing_bvh(config,world);
    }
    
    io.clear_scene(sceneSpheres);
}
BENCHMARK(BM_BVH_Tracing_at_threadNum)->RangeMultiplier(2)->Range(1,32)->Unit(benchmark::kMillisecond);



BENCHMARK_MAIN();