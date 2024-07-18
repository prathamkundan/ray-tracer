#pragma once

#include "point.hpp"
#include <glm/vec3.hpp>

using glm::vec3;

class ray {
  ray(const point3 &origin, const vec3 &direction)
      : orig(origin), dir(direction) {}

  const point3 &origin() const { return orig; }
  const vec3 &direction() const { return dir; }

private:
  point3 orig;
  vec3 dir;
};
