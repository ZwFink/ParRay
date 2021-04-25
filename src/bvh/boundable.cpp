#include "boundable.h"
#include "vec3.h"
#include <cfloat>

Extent::Extent()
{
    for (uint8_t i = 0; i < kNumPlaneSetNormals; ++i)
    {
        d[i][0] = DBL_MAX;
        d[i][1] = -DBL_MAX;
    }
}

void Extent::extendBy(const Extent &extents)
{
    for (int i = 0; i < kNumPlaneSetNormals; ++i)
    {
        if (extents.d[i][0] < d[i][0])
            d[i][0] = extents.d[i][0];
        if (extents.d[i][1] > d[i][1])
            d[i][1] = extents.d[i][1];
    }
}

// Here we are implementing the formular to calculate t the time a ray needs to hit a normal plane.
// t_near = (d_near - N*O)/(N*R)
bool Extent::interset(const double *numberator, const double *denominator, double &tNear, double &tFar, int &planeIndex)
{
    for (int i = 0; i < kNumPlaneSetNormals; i++)
    {
        double tn = (d[i][0] - numberator[i]) / denominator[i];
        double tf = (d[i][1] - numberator[i]) / denominator[i];
        if (denominator[i]==0){
            continue;
        }
        if (denominator[i] < 0)
            std::swap(tn, tf);
        if (tn > tNear)
            tNear = tn, planeIndex = i;
        if (tf < tFar)
            tFar = tf;
        if (tNear > tFar)
            return false;
    }
    return true;
}

vec3 Extent::centroid() const
{
    return vec3(
        (d[0][0] + d[0][1]) * 0.5,
        (d[1][0] + d[1][1]) * 0.5,
        (d[2][0] + d[2][1]) * 0.5);
}

Sphere::Sphere(vec3 _center, double _r) : Sphere(_center, _r, shared_ptr<material>())
{
}

Sphere::Sphere(vec3 _center, double _r, shared_ptr<material> _m): center(_center), r(_r), mat_ptr(_m){
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

bool Sphere::hit(const ray& ray, const double t_min, const double t_max, hit_record &rec) const{
   vec3 oc = ray.origin() - this->center;
   auto a = ray.direction().length_squared();
   auto half_b = dot(oc, ray.direction());
   auto c = oc.length_squared() - r*r;

   auto discriminant = half_b*half_b - a*c;
   if(discriminant < 0) return false;

  auto sqrtd = sqrt(discriminant);

  // find the nearest root that lies in the acceptable range
  auto root = (-half_b - sqrtd) / a;

  if(root < t_min || t_max < root)
  {
      root = (-half_b + sqrtd) / a;
      if(root < t_min || t_max < root)
        return false;
  }

  rec.t = root;
  rec.p = ray.at(rec.t);
  vec3 outward_normal = (rec.p - this->center) / r;
  rec.set_face_normal(ray, outward_normal);
  rec.mat_ptr = this->mat_ptr;
  return true;
}