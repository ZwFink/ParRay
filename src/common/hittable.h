#ifndef HITTABLE_HH_INCLUDED
#define HITTABLE_HH_INCLUDED
#include "ray.h"

class material;

struct hit_record
{
  point3 p;
  // surface norm of the hit
  vec3 normal;
  shared_ptr<material> mat_ptr;
  double t;
  bool front_face;

  inline void set_face_normal(const ray& r, const vec3& outward_normal)
  {
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

class hittable
{
 public:
  // Is there a t_min < t < t_max such that ray's P(t) intersects with the object?
  virtual bool hit(const ray&r, double t_min, double t_max, hit_record& rec) const = 0;
};


#endif // HITTABLE_HH_INCLUDED
