#ifndef __H__SPHERE_GEN__
#define __H__SPHERE_GEN__
#include <vector>
#include "sphere.h"
#include "boundable.h"

class SphereGeneration
{
    public:
    std::vector<sphere*> random_scene_spheres(int size);
    std::vector<Sphere*> random_scene_Spheres(int size);
};
#endif