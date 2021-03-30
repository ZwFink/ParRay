#ifndef COLOR_HH_INCLUDED
#define COLOR_HH_INCLUDED

#include "vec3.h"
#include <iostream>

void write_color(std::ostream &out, color pixel_color)
{
  out << static_cast<int>(255.999 * pixel_color.x()) << ' '
      << static_cast<int>(255.999 * pixel_color.y()) << ' '
      << static_cast<int>(255.999 * pixel_color.z()) << '\n';

}

#endif // color_hh_included
