#ifndef COLOR_H
#define COLOR_H

#include <cmath>
#include <iostream>

#include <vec3.hpp>
#include "interval.hpp"

using color3 = vec3;

inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0) return sqrt(linear_component);
    return 0;
}

inline void write_color(std::ostream &out, const color3 &color) {
    auto r = linear_to_gamma(color.x());
    auto g = linear_to_gamma(color.y());
    auto b = linear_to_gamma(color.z());

    static const interval intensity(0.0, 0.999);

    out << int(256 * intensity.clamp(r)) << " " << int(256 * intensity.clamp(g))
        << " " << int(256 * intensity.clamp(b)) << "\n";
}

#endif
