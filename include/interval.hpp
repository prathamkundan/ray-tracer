#pragma once

#include <cstdio>
#include "utils.hpp"

class interval {
   public:
    double min, max;

    HD interval() {
        min = -inf;
        max = inf;
    }  // Default interval is empty

    HD interval(double min, double max) {
        this->min = min;
        this->max = max;
    }

    HD double size() const { return max - min; }

    HD bool contains(double x) const { return min <= x && x <= max; }

    HD bool surrounds(double x) const { return min < x && x < max; }

    HD double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    static const interval empty, universe;
};

inline const interval interval::empty = interval(+inf, -inf);
inline const interval interval::universe = interval(-inf, +inf);
