#include "camera.hpp"
#include "common.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"

int main() {
  hittable_list world;

    auto material_ground = make_shared<lambertian>(color3(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color3(0.1, 0.2, 0.5));
    auto material_left   = make_shared<metal>(color3(0.8, 0.8, 0.8));
    auto material_right  = make_shared<metal>(color3(0.8, 0.6, 0.2));

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.2),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

  camera camera;

  camera.samples_per_pixel = 100;
  camera.image_width = 800;
  camera.render(world);
}
