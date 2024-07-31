#pragma once

#include <curand_kernel.h>
#include <curand_uniform.h>
#include <cstdio>

#include "../common.hpp"
#include "cu_hittable.hpp"
#include "cu_material.hpp"

class cu_camera {
   public:
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    double focal_length = 1.0;
    double viewport_height = 2.0;
    int samples_per_pixel = 20;
    int max_depth = 10;

    double fov = 90.0f;
    point3 lookfrom = point3(0, 0, 0);
    point3 lookat = point3(0, 0, -1);
    vec3 vup = vec3(0, 1, 0);

    double defocus_angle = 0.0;
    double focus_distance = 10;

    cu_camera() {};
    cu_camera(double aspect_ratio, int image_width, double viewport_height,
                 double focal_length, int samples_per_pixel)
        : aspect_ratio(aspect_ratio),
          image_width(image_width),
          viewport_height(viewport_height),
          focal_length(focal_length),
          samples_per_pixel(samples_per_pixel) {
        pixel_samples_scale = 1.0 / samples_per_pixel;
    };

    HD void render(const cu_hittable &world);

    int image_height;    // Rendered image height
    point3 center;       // Camera center
    point3 pixel00_loc;  // Location of pixel 0, 0
    vec3 pixel_delta_u;  // Offset to pixel to the right
    vec3 pixel_delta_v;
    float pixel_samples_scale;
    vec3 u, v, w;
    vec3 defocus_disk_u;
    vec3 defocus_disk_v;
    curandStateXORWOW_t rand_state;

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // Determine viewport dimensions.
        auto focal_length = (lookfrom - lookat).length();
        auto viewport_height =
            2 * tan(degrees_to_radians(fov / 2)) * focus_distance;
        auto viewport_width =
            viewport_height * (double(image_width) / image_height);

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical
        // viewport edges.
        auto viewport_u = viewport_width * u;
        auto viewport_v = -viewport_height * v;

        // Calculate the horizontal and vertical delta vectors from pixel to
        // pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left =
            center - (focus_distance * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc =
            viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        auto defocus_radius =
            focus_distance * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    };

    __device__ void initRandState() {
        curand_init(42, 0, 0, &rand_state);
    }

    __device__ color3 ray_color(const ray &r, const cu_hittable *world, int depth) {
        printf("ray_color depth: %d\n", depth);
        if (depth <= 0) return color3(0, 0, 0);

        cu_hit_record rec;
        if (world->hit(r, interval(0.001, inf), rec)) {
            ray scattered;
            color3 attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered, &rand_state))
                return attenuation * ray_color(scattered, world, depth - 1);

            return color3(0, 0, 0);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color3(1.0, 1.0, 1.0) + a * color3(0.5, 0.7, 1.0);
    }

    __device__ ray get_ray(int i, int j) {
        // Construct a camera ray originating from the origin and directed at
        // randomly sampled point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) +
                            ((j + offset.y()) * pixel_delta_v);

        // auto ray_origin = defocus_angle <= 0 ? center : defocus_disk_sample();
        auto ray_origin = center;
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    };

    __device__ point3 defocus_disk_sample() {
        // Returns a random point in the camera defocus disk.
        auto p = cu_random_in_unit_disk(&rand_state);
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    };

    __device__ vec3 sample_square() {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit
        // square.
        return vec3(curand_uniform_double(&rand_state) - 0.5, curand_uniform_double(&rand_state) - 0.5, 0);
    }

    cu_camera* clone() {
        return new cu_camera(*this);
    }
};
