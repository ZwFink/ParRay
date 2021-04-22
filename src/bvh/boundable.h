#ifndef __H_BOUNDABLE__
#define __H_BOUNDABLE__

#include "bvh.hpp"
#include "vec3.h"

class Boundable{
    public:
    /**
     * @brief calculate the Extent (bounds) of the object
     * @param[in] normalPlanes a set of normal planes to calculate the bounds 
     * @param[in] origin the origin the extent is relative to
     * @param[out] outputExtent the resulting extent
     */
    virtual void calculateBounds(const vec3 normalPlanes[], const int planeSize, const vec3 origin, Extent &outputExtent)=0;
};

class Sphere: virtual Boundable{
    public:
    Sphere(vec3 center, double r);
    void calculateBounds(const vec3 normalPlanes[], const int planeSize, const vec3 origin, Extent &outputExtent) override;
    private:
    vec3 center;
    double r;
};

#endif