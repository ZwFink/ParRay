#ifndef SPHERE_HH_INCLUDED
#define SPHERE_HH_INCLUDED

#include "hittable.h"
#include "vec3.h"
#include "material.h"


class sphere : public hittable
{
public:
  sphere() {}
  sphere(point3 cen, double r, unique_ptr<material> m)
    : center{cen}, radius{r}, mat_ptr{std::move(m)}
  {}

  virtual bool hit(const ray& r, double t_min,
                   double t_max, hit_record& rec)
    const override;

public:
  point3 center;
  double radius;
  unique_ptr<material> mat_ptr;
};

#endif // SPHERE_HH_INCLUDED
