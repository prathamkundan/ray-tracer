#pragma once

#include "utils.hpp"

class interval {
   public:
    double min, max;

    interval() : min(+inf), max(-inf) {}  // Default interval is empty

    interval(double min, double max) : min(min), max(max) {}

    double size() const { return max - min; }

    bool contains(double x) const { return min <= x && x <= max; }

    bool surrounds(double x) const { return min < x && x < max; }

    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    static const interval empty, universe;
};

inline const interval interval::empty = interval(+inf, -inf);
inline const interval interval::universe = interval(-inf, +inf);
