#pragma once

#include <cstdio>
#include "cu_hittable.hpp"

class cu_sphere : public cu_hittable {
   public:
    __device__ cu_sphere(const point3& center, double radius, cu_material* mat)
        : center(center), radius(cu_max(0, radius)), mat(mat) {
            // printf("In cu_sphere\n");
            // printf("radius: %f\n", radius);
            // printf("center: %f, %f, %f\n", center.x(), center.y(), center.z());
        }

    __device__ bool hit(const ray& r, interval ray_t,
                cu_hit_record& rec) const override {
        // printf("->sphere hit\n");
        vec3 oc = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = h * h - a * c;
        if (discriminant < 0) {
            // printf("skipped\n");
            return false;
        }

        auto sqrtd = sqrtf(discriminant);

        // Find the nearest root that lies in the acceptable range.
        // printf("root finding\n");
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root)) return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);

        // printf("Setting normal\n");
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

    virtual cu_hittable* clone() const override {
        return new cu_sphere(*this);
    }

   private:
    point3 center;
    double radius;
    cu_material* mat;
};
