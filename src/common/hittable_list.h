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
#endif // HITTABLE_LIST_HH_INCLUDED
