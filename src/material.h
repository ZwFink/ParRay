#ifndef MATERIAL_HH_INCLUDED
#define MATERIAL_HH_INCLUDED

#include "common.h"

struct hit_record;

class material
{
public:
  virtual bool scatter(
                       const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
                       ) const = 0;

};

#endif // MATERIAL_HH_INCLUDED
