#pragma once

#include <vector>
#include "cu_camera.hpp"
#include "cu_material.hpp"

class Allocator {
   public:
    cu_hittable** allocate_sphere(const point3 center, double radius,
                               cu_material** mat);

    cu_hittable** allocate_list();

    cu_material** allocate_metal(const color3 albedo, double fuzz);

    cu_material** allocate_lambertian(color3 albedo);

    cu_material** allocate_dielectric(double refraction_index);

    void test(cu_camera cam, color3* output);

    std::vector<cu_material**> allocated_materials;
    std::vector<cu_hittable**> allocated_hittables;
    cu_hittable** world;
};
