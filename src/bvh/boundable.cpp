#include "boundable.h"
#include "vec3.h"
#include "bvh.hpp"

Sphere::Sphere(vec3 _center, double _r) : center(_center), r(_r)
{
}

void Sphere::calculateBounds(const vec3 normalPlanes[], const int planeSize, const vec3 origin, Extent &outputExtent)
{
    for (int i = 0; i < planeSize; i++)
    {
        vec3 normal = normalPlanes[i];
        vec3 unit_normal = unit_vector(normal);
        double d1 = dot(origin + center + r * unit_normal, unit_normal);
        double d2 = dot(origin + center - r * unit_normal, unit_normal);
        outputExtent.d[i][0] = std::min(d1, d2);
        outputExtent.d[i][1] = std::max(d1, d2);
    }
}
