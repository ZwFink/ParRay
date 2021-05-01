#ifndef __H_RAY_TRACING__
#define __H_RAY_TRACING__

#include "bvh.hpp"
#include "camera.h"
#include "color.h"

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

void raytracing_bvh(const traceConfig &config);
color ray_color(const ray &r, BVH &world, int depth);
#endif