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
    int threadNumber;
    color* out_image;
    traceConfig(camera &_cam, BVH &_world, int _width, int _height, int _depth, int _sample, int _threadNum):cam(_cam), world(_world), width(_width), height(_height), traceDepth(_depth), samplePerPixel(_sample), threadNumber(_threadNum){
      out_image = new color[height * width];
    }
    ~traceConfig(){
      delete[] out_image;
    }
};

void raytracing_bvh(const traceConfig &config);
#endif