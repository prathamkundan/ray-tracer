#pragma once

#include "common.hpp"
#include "hittable.hpp"

class material {
   public:
    HD virtual ~material() = default;

    HD virtual bool scatter(const ray& r_in, const hit_record& rec,
                            color3& attenuation, ray& scattered) const {
        return false;
    }
};

class cu_lambertian : public material {
   public:
    HD cu_lambertian(const color3& albedo) : albedo(albedo) {}

    HD virtual bool scatter(const ray& r_in, const hit_record& rec,
                            color3& attenuation,
                            ray& scattered) const override {
        auto scatter_direction = rec.normal + random_unit_vector();
        if (scatter_direction.near_zero()) scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

   private:
    color3 albedo;
};

class cu_metal : public material {
   public:
    HD cu_metal(const color3& albedo, double fuzz) : albedo(albedo), fuzz(fuzz) {}

    HD virtual bool scatter(const ray& r_in, const hit_record& rec,
                            color3& attenuation,
                            ray& scattered) const override {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

   private:
    color3 albedo;
    double fuzz;
};

class cu_dielectric : public material {
   public:
    HD cu_dielectric(double refraction_index)
        : refraction_index(refraction_index) {}

    HD bool scatter(const ray& r_in, const hit_record& rec, color3& attenuation,
                    ray& scattered) const override {
        attenuation = color3(1.0, 1.0, 1.0);
        double ri =
            rec.front_face ? (1.0 / refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.p, direction);
        return true;
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
