#ifndef __H_RAY_TRACING__
#define __H_RAY_TRACING__

#include "bvh.hpp"
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "hittable.h"

struct traceConfig
{
    camera& cam;
    bool printOutput;
    int width;
    int height;
    int traceDepth;
    int samplePerPixel;
    int numProcs;
    int myRank;
    int threadsPerProc;
    traceConfig(camera &_cam, int _width, int _height, int _depth, int _sample, int _numProcs, int _myRank, int _threads_per_proc): traceConfig(_cam, _width, _height, _depth, _sample, _numProcs, _myRank, _threads_per_proc, false){}
    traceConfig(camera &_cam, int _width, int _height, int _depth, int _sample, int _numProcs, int _myRank, int _threads_per_proc, bool _print_output)    :cam(_cam), width(_width), height(_height), traceDepth(_depth), samplePerPixel(_sample), numProcs(_numProcs), myRank(_myRank), threadsPerProc(_threads_per_proc), printOutput(_print_output){}
};

/**
 * @brief openmp version of bvh tracing
 */
void raytracing_bvh(const traceConfig &config, BVH &world);

/**
 * @brief a single thread bvh tracing for debug and profiling purpose. This method is 
 * free of mpi or openmp premitives.
 */
void raytracing_bvh_single_threaded(const traceConfig &config, BVH &world);

void raytracing_hittablelist(const traceConfig &config, hittable_list &world);

bool getTileIndexes(const int width, const int height, const int tileSize, const int id, int &startRow, int &startCol, int &endRow, int &endCol);
void raytracing_bvh_tiled(const traceConfig &config, BVH &world, const int tileSize);

#endif