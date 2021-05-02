#ifndef __H_RAY_TRACING__
#define __H_RAY_TRACING__

#include "bvh.hpp"
#include "camera.h"
#include "color.h"
#include "hittable.h"
#include "hittable_list.h"

struct traceConfig
{
    camera& cam;
    BVH& world;
    int width;
    int height;
    int traceDepth;
    int samplePerPixel;
    int numProcs;
    int myRank;
    int threadsPerProc;
    traceConfig(camera &_cam, BVH &_world, int _width, int _height, int _depth, int _sample, int _numProcs, int _myRank, int _threads_per_proc)
    :cam(_cam), world(_world), width(_width), height(_height), traceDepth(_depth), samplePerPixel(_sample),
     numProcs(_numProcs), myRank(_myRank), threadsPerProc(_threads_per_proc)
  {}
};

/**
 * @brief openmp version of bvh tracing
 */
void raytracing_bvh(const traceConfig &config);

/**
 * @brief a single thread bvh tracing for debug and profiling purpose. This method is 
 * free of mpi or openmp premitives.
 */
void raytracing_bvh_single_threaded(const traceConfig &config);
void raytracing_bvh_single_threaded_debug(const traceConfig &config);


color ray_color(const ray &r, BVH &world, hittable_list &simpleWorld, int depth);
color ray_color(const ray &r, BVH &world, int depth);
#endif