#include <cstdlib>

#include "cuda/cu_allocate.hpp"
#include "cuda/cu_camera.hpp"
#include "device_helper.hpp"

int main1() {
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

    std::cout << "P3\n" << cam.image_width << ' ' << cam.image_height <<
    "\n255\n";

    for (int i=0; i<cam.image_height * cam.image_width; i++)
        write_color(std::cout, output[i]);


    return 0;

}

int main() {
    Allocator world;

    auto ground_material = world.allocate_lambertian(color3(0.5, 0.5, 0.5));
    world.allocate_sphere(point3(0,-1000,0), 1000, ground_material);

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                cu_material** sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color3::random() * color3::random();
                    sphere_material = world.allocate_lambertian(albedo);
                    world.allocate_sphere(center, 0.2, sphere_material);
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color3::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = world.allocate_metal(albedo, fuzz);
                    world.allocate_sphere(center, 0.2, sphere_material);
                } else {
                    // glass
                    sphere_material = world.allocate_dielectric(1.5);
                    world.allocate_sphere(center, 0.2, sphere_material);
                }
            }
        }
    }

    auto material1 = world.allocate_dielectric(1.5);
    world.allocate_sphere(point3(0, 1, 0), 1.0, material1);

    auto material2 = world.allocate_lambertian(color3(0.4, 0.2, 0.1));
    world.allocate_sphere(point3(-4, 1, 0), 1.0, material2);

    auto material3 = world.allocate_metal(color3(0.7, 0.6, 0.5), 0.0);
    world.allocate_sphere(point3(4, 1, 0), 1.0, material3);

    world.allocate_list();

    cu_camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1200;
    cam.samples_per_pixel = 500;
    cam.max_depth         = 50;

    cam.fov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_distance    = 10.0;

    cam.initialize();

    color3* output = new color3[cam.image_height * cam.image_width];
    world.test(cam, output);

    std::cout << "P3\n" << cam.image_width << ' ' << cam.image_height <<
    "\n255\n";

    for (int i=0; i<cam.image_height * cam.image_width; i++)
        write_color(std::cout, output[i]);

    return 0;
}

