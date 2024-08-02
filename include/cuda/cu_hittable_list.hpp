#pragma once

#include <cstdio>
#include "cu_hittable.hpp"

class cu_hittable_list : public cu_hittable {
   public:
    cu_hittable ***objects;
    int num_objects;

    __device__ cu_hittable_list() { }

    __device__ void set_objects(cu_hittable ***d_objects, int d_num_objects) {
        objects = d_objects;
        num_objects = d_num_objects;
    }

    __device__ bool hit(const ray &r, interval ray_t,
                cu_hit_record &rec) const override {
        // printf("-> list hit\n");
        cu_hit_record temp_rec;
        bool hit_anything = false;

        double closest_so_far = ray_t.max;

        // printf("pre-loop\n");
        for (int i = 0; i < num_objects; i++) {
            // printf("Object: %d\n", i);
            cu_hittable **object = objects[i];

            // printf("obj-ref\n");
            if ((*object)->hit(r, interval(ray_t.min, closest_so_far),
                               temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    virtual cu_hittable *clone() const override {
        return new cu_hittable_list(*this);
    }
};
