#ifndef __H_BOUNDABLE__
#define __H_BOUNDABLE__

#include "vec3.h"
#include "hittable.h"
#include "material.h"

const int kNumPlaneSetNormals = 7;

class Boundable;
class Sphere;

class Extent
{
public:
    Extent();
    void extendBy(const Extent &extents);
    bool interset(const double *numberator, const double *denominator, double &tNear, double &tFar, int &planeIndex);
    vec3 centroid() const;

public:
    double d[kNumPlaneSetNormals][2]; //the distance d values for each plane set normals
    Sphere *object;
};

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

class Sphere: public Boundable, public hittable{
    public:
    Sphere(vec3 center, double r);
    Sphere(vec3 center, double r, shared_ptr<material> material);
    void calculateBounds(const vec3 normalPlanes[], const int planeSize, const vec3 origin, Extent &outputExtent) override;
    bool hit(const ray& ray, double t_min, double t_max, hit_record &rec) const override;
    vec3 center;
    std::shared_ptr<material> mat_ptr;
    double r;
};

#endif