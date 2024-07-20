#pragma once

#include "common.hpp"
#include "hittable.hpp"

class sphere : public hittable {
public:
  sphere(const point3 &center, double radius, const shared_ptr<material> mat)
      : center(center), radius(fmax(0, radius)), mat(mat) {}

  bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
    vec3 oc = center - r.origin();
    auto a = dot(r.direction(), r.direction());
    auto h = dot(r.direction(), oc);
    auto c = dot(oc, oc) - radius * radius;
    auto discriminant = h * h - a * c;
    if (discriminant < 0) {
      return false;
    }
    auto root = (h - sqrt(discriminant)) / a;
    if (!ray_t.surrounds(root)) {
      root = (h + sqrt(discriminant)) / a;
      if (!ray_t.surrounds(root))
        return false;
    }

    rec.t = root;
    rec.p = r.at(root);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat = mat;

    return true;
  }

private:
  point3 center;
  double radius;
  shared_ptr<material> mat;
};
