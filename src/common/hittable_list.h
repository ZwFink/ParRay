#ifndef HITTABLE_LIST_HH_INCLUDED
#define HITTABLE_LIST_HH_INCLUDED
#include "hittable.h"
#include <memory>
#include <vector>

using std::unique_ptr;
using std::make_unique;

class hittable_list : public hittable
{
 public:
  hittable_list() {}
  hittable_list(std::unique_ptr<hittable> object) { add(std::move(object)); }

  void clear() { objects.clear(); }
  void add(std::unique_ptr<hittable> object) { objects.emplace_back(std::move(object)); }

  virtual bool hit(
                   const ray& r, double t_min, double t_max, hit_record& rec) const override;

public:
  std::vector<unique_ptr<hittable>> objects;
};

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
  hit_record temp_rec;
  bool hit_anything = false;

  auto closest_so_far = t_max;

  for(const auto& object : objects)
    {
      if(object->hit(r, t_min, closest_so_far, temp_rec))
        {
          hit_anything = true;
          closest_so_far = temp_rec.t;
          rec = temp_rec;
        }
    }
  return hit_anything;
}

#endif // HITTABLE_LIST_HH_INCLUDED
