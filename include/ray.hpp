#pragma once

#include "vec3.hpp"

class ray {
   public:
    HD ray() {};

    HD ray(const point3 &origin, const vec3 &direction)
        : orig(origin), dir(direction) {}

    HD const point3 &origin() const { return orig; }
    HD const vec3 &direction() const { return dir; }

    HD point3 at(float t) const { return orig + (t * dir); }

   private:
    point3 orig;
    vec3 dir;
};
