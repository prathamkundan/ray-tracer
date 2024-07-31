#pragma once

#include <curand_kernel.h>
#include <curand_uniform.h>
#include <cstdio>

#include "../common.hpp"
#include "cu_hittable.hpp"

class cu_material {
   public:
    HD virtual ~cu_material() = default;

    __device__ virtual bool scatter(const ray& r_in, const cu_hit_record& rec,
                                    color3& attenuation, ray& scattered,
                                    curandState* rand_state) const {
        return false;
    }

    virtual cu_material* clone() const = 0;
};

class cu_lambertian : public cu_material {
   public:
    HD cu_lambertian(const color3& albedo) : albedo(albedo) {}

    __device__ virtual bool scatter(const ray& r_in, const cu_hit_record& rec,
                                    color3& attenuation, ray& scattered,
                                    curandState* rand_state) const override {
        auto scatter_direction = rec.normal + cu_random_unit_vector(rand_state);
        if (scatter_direction.near_zero()) scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

    virtual cu_material* clone() const override {
        return new cu_lambertian(*this);
    }

   private:
    color3 albedo;
};

class cu_metal : public cu_material {
   public:
    HD cu_metal(const color3& albedo, double fuzz)
        : albedo(albedo), fuzz(fuzz) {}

    __device__ virtual bool scatter(const ray& r_in, const cu_hit_record& rec,
                                    color3& attenuation, ray& scattered,
                                    curandState* rand_state) const override {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected =
            unit_vector(reflected) + (fuzz * cu_random_unit_vector(rand_state));
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

    virtual cu_material* clone() const override { return new cu_metal(*this); }

   private:
    color3 albedo;
    double fuzz;
};

class cu_dielectric : public cu_material {
   public:
    HD cu_dielectric(double refraction_index)
        : refraction_index(refraction_index) {}

    __device__ bool scatter(const ray& r_in, const cu_hit_record& rec,
                            color3& attenuation, ray& scattered,
                            curandState* rand_state) const override {
        attenuation = color3(1.0, 1.0, 1.0);
        double ri =
            rec.front_face ? (1.0 / refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract ||
            reflectance(cos_theta, ri) > curand_uniform_double(rand_state))
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.p, direction);
        return true;
    }

    virtual cu_material* clone() const override {
        return new cu_dielectric(*this);
    }

   private:
    // Refractive index in vacuum or air, or the ratio of the material's
    // refractive index over the refractive index of the enclosing media
    double refraction_index;

    HD static double reflectance(double cosine, double refraction_index) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};
