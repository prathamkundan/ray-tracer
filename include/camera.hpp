#pragma once

#include "common.hpp"
#include "hittable.hpp"

class camera {
public:
  double aspect_ratio = 16.0 / 9.0;
  int image_width = 400;
  double focal_length = 1.0;
  double viewport_height = 2.0;

  camera(){};
  camera(double aspect_ratio, int image_width, double viewport_height,
         double focal_length)
      : aspect_ratio(aspect_ratio), image_width(image_width),
        viewport_height(viewport_height), focal_length(focal_length){};

  void render(const hittable &world);

private:
  int image_height;   // Rendered image height
  point3 center;      // Camera center
  point3 pixel00_loc; // Location of pixel 0, 0
  vec3 pixel_delta_u; // Offset to pixel to the right
  vec3 pixel_delta_v;

  void initialize();
  color3 ray_color(const ray &r, const hittable &world);
};
