#ifndef __H__SPHERE_GEN__
#define __H__SPHERE_GEN__
#include <vector>
#include "sphere.h"
#include "boundable.h"
#include "hittable_list.h"

class SphereGeneration
{
    public:
    hittable_list random_scene_hittablelist(int size);
    std::vector<sphere*> random_scene_spheres(int size);
    std::vector<Sphere*> random_scene_Spheres(int size);
};
#endif