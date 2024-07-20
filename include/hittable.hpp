#pragma once

#include "color.hpp"
#include "common.hpp"
#include "vec3.hpp"
class material;

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
    void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class material {
   public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in, const hit_record& rec,
                         color3& attenuation, ray& scattered) const {
        return false;
    }
};

class lambertian : public material {
   public:
    lambertian(const color3& albedo) : albedo(albedo) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec,
                         color3& attenuation, ray& scattered) const override {
        auto scatter_direction = rec.normal + random_unit_vector();
        if (scatter_direction.near_zero()) scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

   private:
    color3 albedo;
};

class metal : public material {
   public:
    metal(const color3& albedo) : albedo(albedo) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec,
                         color3& attenuation, ray& scattered) const override {
        auto reflected_direction = reflect(r_in.direction(), rec.normal);

        scattered = ray(rec.p, reflected_direction);
        attenuation = albedo;
        return true;
    }

   private:
    color3 albedo;
};

class hittable {
   public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};
