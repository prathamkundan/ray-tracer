#pragma once

class material;

#include "common.hpp"
#include "vec3.hpp"

class hit_record {
   public:
    point3 p;
    vec3 normal;
    shared_ptr<material> mat;
    double t;
    bool front_face;

    /* Sets the hit record normal vector.
     * NOTE: the parameter `outward_normal` is assumed to have unit length.
     */
    void set_face_normal(const ray& r, const vec3& outward_normal);
};

class hittable {
   public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};
