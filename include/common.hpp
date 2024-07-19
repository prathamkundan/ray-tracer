#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>


// C++ Std Usings

using std::make_shared;
using std::shared_ptr;
using std::vector;
using std::sqrt;

// Constants

inline const double inf = std::numeric_limits<double>::infinity();
inline const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

// Common Headers

#include "color.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include "interval.hpp"
