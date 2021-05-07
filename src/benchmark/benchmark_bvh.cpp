#include <benchmark/benchmark.h>
#include "ray_tracing.h"
#include "data_porting.h"
#include "vec3.h"
#include "camera.h"
#include "sphere_generation.h"

/*
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

static void BM_Baseline_BVH_Tracing_at_sceneSize(benchmark::State &state)
{
    int size = state.range(0);
    ShapeDataIO io;

    camera cam = camera::getDefault();
    // Image
    const int image_width = 100;
    const int image_height = static_cast<int>(image_width / cam.aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 4;
    int num_thread=1;
    //create scene
    SphereGeneration sphereGen;
    std::vector<Sphere*> spheres = sphereGen.random_scene_Spheres(size);
    BVH world(spheres);
    //create config
    traceConfig config(cam, image_width, image_height, max_depth, samples_per_pixel, num_thread,0,1);

    for (auto _ : state){
     raytracing_bvh(config,world);
    }
    io.clear_scene(spheres);
}
BENCHMARK(BM_Baseline_BVH_Tracing_at_sceneSize)->Unit(benchmark::kMillisecond)->RangeMultiplier(2)->Range(1,10);
*/

static void BM_BVH_Tracing_OMP_threadsNum_tileSize(benchmark::State &state)
{
    int num_thread=state.range(0);
    int tileSize = state.range(1);
    ShapeDataIO io;

    camera cam = camera::getDefault();
    // Image
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / cam.aspect_ratio);
    const int samples_per_pixel = 500;
    const int max_depth = 10;
    //create scene
    SphereGeneration sphereGen;
    std::vector<Sphere*> spheres = sphereGen.random_scene_Spheres(20);
    BVH world(spheres);
    //create config
    traceConfig config(cam, image_width, image_height, max_depth, samples_per_pixel, num_thread,0,1);

    for (auto _ : state){
     raytracing_bvh_tiled(config,world,tileSize);
    }
    io.clear_scene(spheres);
}
BENCHMARK(BM_BVH_Tracing_OMP_threadsNum_tileSize)->Unit(benchmark::kMillisecond)->RangeMultiplier(2)->Ranges({{4, 64}, {1, 64}});

static void BM_BVH_Tracing_at_threadNum(benchmark::State &state)
{
    ShapeDataIO io;

    SphereGeneration sphereGen;
    std::vector<Sphere*> spheres = sphereGen.random_scene_Spheres(20);

    camera cam = camera::getDefault();
    // Image
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / cam.aspect_ratio);
    const int samples_per_pixel = 500;
    const int max_depth = 10;
    BVH world(spheres);

    int num_threads = state.range(0);
    traceConfig config(cam, image_width, image_height, max_depth, samples_per_pixel, num_threads,0,1);

    for (auto _ : state){
     raytracing_bvh(config,world);
    }
    
    io.clear_scene(spheres);
}
BENCHMARK(BM_BVH_Tracing_at_threadNum)->RangeMultiplier(2)->Range(4,64)->Unit(benchmark::kMillisecond);



BENCHMARK_MAIN();
