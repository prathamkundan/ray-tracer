#pragma once

#include <cmath>
#include <cstdlib>
#include <utility>
#include <cuda_runtime.h>
#include <curand.h>
#include <curand_kernel.h>
#include <curand_uniform.h>

#define HD __host__ __device__

inline const double inf = (double) INFINITY;
inline const double pi = 3.1415926535897932385;

HD inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}


HD inline double random_double() { return rand() / (RAND_MAX + 1.0); }

HD inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

__device__ inline double cu_random_double(curandState* rand_state) {
    return curand_uniform_double(rand_state);
}

__device__ inline double cu_random_double(double min, double max, curandState* rand_state) {
    return min + (max - min) * cu_random_double(rand_state);
}

HD inline double cu_max(double a, double b) {
    return a > b ? a : b;
}
