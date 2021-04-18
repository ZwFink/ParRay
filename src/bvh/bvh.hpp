#ifndef __H_BVH
#define __H_BVH

#include "vec3.h"
#include "hittable.h"
#include "float.h"

extern const int kNumPlaneSetNormals=7;

class Extent
{
    public:
    Extent();
    void extendBy(const Extent &extents);
    bool interset(const double *numberator, const double *denominator, double &tNear, double &tFar, int &planeIndex);

    private:
    double d[kNumPlaneSetNormals][2]; //the distance d values for each plane set normals
    hittable *object;             //pointer contained by the volumn

};



/*
class bvh
{
    static const uint8_t kNumPlaneSetNormals = 7;
    static const vec3 planeSetNormals[kNumPlaneSetNormals];
    struct Extents
    {
        Extents()
        {
            for (uint8_t i = 0; i < kNumPlaneSetNormals; ++i)
            {
                d[i][0] = DBL_MAX;
                d[i][1] = -DBL_MAX;
            }
        }
        void extendBy(const Extents &extents)
        {
            for (int i = 0; i < kNumPlaneSetNormals; ++i)
            {
                if (extents.d[i][0] < d[i][0])
                    d[i][0] = extents.d[i][0];
                if (extents.d[i][1] > d[i][1])
                    d[i][1] = extents.d[i][1];
            }
        }
        bool interset(const double *numberator, const double denominator, double &tNear, double &tFar, int &planeIndex);
        double d[kNumPlaneSetNormals][2]; //the distance d values for each plane set normals
        const Object *object;             //pointer contained by the volumn
    };
    Extents *
} public : bvh();
~bvh();
}
;
bvh::bvh()
{
}

bvh::~bvh()
{
}
*/
#endif