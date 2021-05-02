#ifndef CAMERA_HH_INCLUDED
#define CAMERA_HH_INCLUDED
#include "common.h"

class camera
{
public:
  camera(point3 lookfrom, point3 lookat, vec3 vup,
         double vfov, double aspect_ratio_,
         double aperture, double focus_dist)
  {

    aspect_ratio = aspect_ratio_;

    auto theta = degrees_to_radians(vfov);
    auto h = tan(theta / 2);
    auto viewport_height = 2.0 * h;
    auto viewport_width = aspect_ratio * viewport_height;

    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    origin = lookfrom;
    horizontal = focus_dist * viewport_width * u;
    ;
    vertical = focus_dist * viewport_height * v;
    ;
    lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

    lens_radius = aperture / 2;
  }

  ray get_ray(double s, double t) const
  {
    vec3 rd = lens_radius * random_in_unit_disk();
    vec3 offset = u * rd.x() + v * rd.y();
    return ray(origin + offset,
               lower_left_corner + s * horizontal + t * vertical - origin - offset);
  }

  static camera getDefault()
  {
    // Image
    const auto aspect_ratio = 3.0 / 2.0;
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;
    return camera(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);
  }

public:
  point3 origin;
  point3 lower_left_corner;
  double aspect_ratio;
  vec3 horizontal;
  vec3 vertical;
  vec3 u, v, w;
  double lens_radius;
};

#endif // CAMERA_HH_INCLUDED
