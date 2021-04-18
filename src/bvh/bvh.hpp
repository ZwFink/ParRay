#ifndef __H_BVH
#define __H_BVH

#include "hittable.h"
#include <float.h>

const int kNumPlaneSetNormals = 7;

class Extent
{
public:
    Extent();
    void extendBy(const Extent &extents);
    bool interset(const double *numberator, const double *denominator, double &tNear, double &tFar, int &planeIndex);

private:
    double d[kNumPlaneSetNormals][2]; //the distance d values for each plane set normals
    hittable *object;                 //pointer contained by the volumn
};
#endif