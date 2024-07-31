#include <cstdlib>

#include "cuda/cu_allocate.hpp"
#include "cuda/cu_camera.hpp"
#include "device_helper.hpp"

int main() {
    Allocator a;

    auto material_ground = a.allocate_lambertian(color3(0.8, 0.8, 0.0));
    auto material_center = a.allocate_lambertian(color3(0.1, 0.2, 0.5));
    auto material_left   = a.allocate_dielectric(1.50);
    auto material_bubble = a.allocate_dielectric(1.00 / 1.50);
    auto material_right  = a.allocate_metal(color3(0.8, 0.6, 0.2), 1.0);

    a.allocate_sphere(point3( 0.0, -100.5, -1.0), 100.0, material_ground);
    a.allocate_sphere(point3( 0.0,    0.0, -1.2),   0.5, material_center);
    a.allocate_sphere(point3(-1.0,    0.0, -1.0),   0.5, material_left);
    a.allocate_sphere(point3(-1.0,    0.0, -1.0),   0.4, material_bubble);
    a.allocate_sphere(point3( 1.0,    0.0, -1.0),   0.5, material_right);

    auto world = a.allocate_list();

    cu_camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 20;

    cam.fov = 90;
    cam.lookfrom = point3(-2, 2, 1);
    cam.lookat = point3(0, 0, -1);
    cam.vup = vec3(0, 1, 0);

    cam.initialize();

    color3* output = new color3[cam.image_height * cam.image_width];

    a.test(cam, output);

    // render(cam, output, world);

    // std::cout << "P3\n" << cam.image_width << ' ' << cam.image_height <<
    // "\n255\n";
    //
    // for (int i=0; i<cam.image_height * cam.image_width; i++)
    //     write_color(std::cout, output[i]);
}
