#pragma once

#include <curand_kernel.h>
#include <cmath>
#include <iostream>

#include "utils.hpp"

class vec3 {
   public:
    double e[3];

    HD vec3() {
        e[0] = 0.0;
        e[1] = 0.0;
        e[2] = 0.0;
    }
    HD vec3(double e0, double e1, double e2) {
        e[0] = e0;
        e[1] = e1;
        e[2] = e2;
    }

    HD double x() const { return e[0]; }
    HD double y() const { return e[1]; }
    HD double z() const { return e[2]; }

    HD vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    HD double operator[](int i) const { return e[i]; }
    HD double& operator[](int i) { return e[i]; }

    HD vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    HD vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    HD vec3& operator/=(double t) { return *this *= 1 / t; }

    HD double length() const { return sqrt(length_squared()); }

    HD double length_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    HD bool near_zero() const {
        // Return true if the vector is close to zero in all dimensions.
        auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }

    HD static vec3 random() {
        return vec3(random_double(), random_double(), random_double());
    }

    HD static vec3 random(double min, double max) {
        return vec3(random_double(min, max), random_double(min, max),
                    random_double(min, max));
    }
};

// point3 is just an alias for vec3, but useful for geometric clarity in the
// code.
using point3 = vec3;

// Vector Utility Functions

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

HD inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

HD inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

HD inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

HD inline vec3 operator*(double t, const vec3& v) {
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

HD inline vec3 operator*(const vec3& v, double t) { return t * v; }

HD inline vec3 operator/(const vec3& v, double t) { return (1 / t) * v; }

HD inline double dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

HD inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

HD inline vec3 unit_vector(const vec3& v) { return v / v.length(); }

HD inline vec3 random_in_unit_sphere() {
    while (true) {
        auto p = vec3::random(-1, 1);
        if (p.length_squared() < 1) return p;
    }
}

__device__ inline vec3 cu_random_in_unit_sphere(curandState* rand_state) {
    while (true) {
        auto p = vec3(cu_random_double(1, -1, rand_state),
                      cu_random_double(1, -1, rand_state),
                      cu_random_double(1, -1, rand_state));
        if (p.length_squared() < 1) return p;
    }
}

HD inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

__device__ inline vec3 cu_random_unit_vector(curandState* rand_state) {
    return unit_vector(cu_random_in_unit_sphere(rand_state));
}

HD inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() < 1) return p;
    }
}

__device__ inline vec3 cu_random_in_unit_disk(curandState* rand_state) {
    while (true) {
        auto p = vec3(cu_random_double(-1, 1, rand_state),
                      cu_random_double(-1, 1, rand_state), 0);
        if (p.length_squared() < 1) return p;
    }
}

HD inline vec3 random_on_hemisphere(const vec3& normal) {
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) >
        0.0)  // In the same hemisphere as the normal
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

HD inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

HD inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}
