#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include <device_types.h>
#include <driver_types.h>
#include <cstdio>
#include <iostream>

#include "color.hpp"
#include "device_helper.hpp"

__global__ void add(int n, float* x, float* y) {
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int i = index; i < n; i += stride) y[i] = x[i] + y[i];
}

__global__ void init_rand_states(cu_camera* d_cam) { d_cam->initRandState(); }

__global__ void device_render(cu_hittable** d_world, cu_camera* d_cam,
                              color3* d_ouput) {
    printf("Block: %d, Thread: %d", blockIdx.x, threadIdx.x);
    if (threadIdx.x !=0 || blockIdx.x != 0) return;
    int width = d_cam->image_width;
    int height = d_cam->image_height;

    printf("World ptr ptr: %p\n", d_world);
    printf("World ptr: %p\n", *d_world);
    printf("%d X %d\n", width, height);

    auto r = d_cam->get_ray(0, 0);
    auto h = cu_hit_record();
    (*d_world)->hit(r, interval(0.001, inf), h);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            auto ray = d_cam->get_ray(i, j);
            printf("Ray: %f, %f, %f\n", ray.direction().x(), ray.direction().y(),
                   ray.direction().z());
            printf("%d, %d\n", i, j);
            auto op = d_cam->ray_color(ray, *d_world, d_cam->max_depth);
            d_ouput[i * width + j] = op;
        }
    }
}

void render(cu_camera& cam, color3* output, cu_hittable** world) {
    std::clog << "Hello Render" << std::endl;
    cu_camera* d_cam;

    cudaError_t err = cudaMallocManaged(&d_cam, sizeof(cu_camera));
    if (err != cudaSuccess) {
        std::clog << "Could not allocate camera on the GPU" << std::endl;
        return;
    }

    *d_cam = cam;

    init_rand_states<<<1, 1>>>(d_cam);
    err = cudaDeviceSynchronize();
    if (err != cudaSuccess) {
        std::clog << "Could not initialize random state" << std::endl;
        return;
    }

    color3* d_output;
    err = cudaMallocManaged(
        &d_output, cam.image_width * cam.image_height * sizeof(color3));
    if (err != cudaSuccess) {
        std::clog << "Could not allocate camera on the GPU" << std::endl;
    }

    device_render<<<1, 1>>>(world, d_cam, d_output);
    cudaDeviceSynchronize();

    for (int i = 0; i < cam.image_height * cam.image_width; i++) {
        // std::clog<<d_output[i]<<std::endl;
        output[i] = d_output[i];
    }

    cudaFree(d_output);
    cudaFree(d_cam);
}

void run() {
    int N = 1 << 20;
    float *x, *y;

    // Allocate Unified Memory – accessible from CPU or GPU
    cudaMallocManaged(&x, N * sizeof(float));
    cudaMallocManaged(&y, N * sizeof(float));

    // initialize x and y arrays on the host
    for (int i = 0; i < N; i++) {
        x[i] = 1.0f;
        y[i] = 2.0f;
    }

    int blockSize = 256;
    int numBlocks = (N + blockSize - 1) / blockSize;
    add<<<numBlocks, blockSize>>>(N, x, y);

    // Wait for GPU to finish before accessing on host
    cudaDeviceSynchronize();

    // Check for errors (all values should be 3.0f)
    float maxError = 0.0f;
    for (int i = 0; i < N; i++) {
        maxError = fmax(maxError, fabs(y[i] - 3.0f));
    }
    std::cout << "Max error: " << maxError << std::endl;

    // Free memory
    cudaFree(x);
    cudaFree(y);
}
