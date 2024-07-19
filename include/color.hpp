#ifndef COLOR_H
#define COLOR_H

#include <iostream>

#include <vec3.hpp>

using color3 = vec3;

inline void write_color(std::ostream &out, const color3 &color) {
  out << int(255.999 * color.x()) << " " << int(255.999 * color.y()) << " "
      << int(255.999 * color.z()) << "\n";
}

#endif
