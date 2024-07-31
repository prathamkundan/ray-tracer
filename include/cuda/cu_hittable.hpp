#pragma once

class cu_material;

#include "../common.hpp"

class cu_hit_record {
   public:
    point3 p;
    vec3 normal;
    cu_material* mat;
    double t;
    bool front_face;

    HD cu_hit_record() {};

    /* Sets the hit record normal vector.
     * NOTE: the parameter `outward_normal` is assumed to have unit length.
     */
    HD void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class cu_hittable {
   public:
    __device__ virtual bool hit(const ray& r, interval ray_t,
                                cu_hit_record& rec) const = 0;

    virtual cu_hittable* clone() const = 0;
};
