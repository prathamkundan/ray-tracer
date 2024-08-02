#include <cuda_runtime_api.h>
#include <cstdio>
#include <iostream>
#include "color.hpp"
#include "cuda/cu_camera.hpp"
#include "cuda/cu_hittable.hpp"
#include "cuda/cu_hittable_list.hpp"
#include "cuda/cu_material.hpp"
#include "cuda/cu_sphere.hpp"
#include "cuda/cu_allocate.hpp"
#include "utils.hpp"

__global__ void cu_allocate_sphere(const point3* center, double radius,
                                   cu_material** mat,
                                   cu_hittable** sphere_ptr) {
    *sphere_ptr = new cu_sphere(*center, radius, *mat);
}

cu_hittable** Allocator::allocate_sphere(const point3 center, double radius,
                                         cu_material** mat) {
    cu_hittable** d_sphere;

    auto err = cudaMallocManaged(&d_sphere, sizeof(cu_hittable*));
    if (err != cudaSuccess) {
        std::cerr << "Could not allocate sphere::cudaMalloc failed"
                  << std::endl;
        std::cerr << "CUDA error: " << cudaGetErrorString(err) << std::endl;
        return nullptr;
    }

    vec3* d_center;
    err = cudaMallocManaged(&d_center, sizeof(vec3));
    if (err != cudaSuccess) {
        std::cerr << "Could not allocate vec3::cudaMalloc failed" << std::endl;
        std::cerr << "CUDA error: " << cudaGetErrorString(err) << std::endl;
        return nullptr;
    }
    *d_center = center;

    cu_allocate_sphere<<<1, 1>>>(d_center, radius, mat, d_sphere);

    err = cudaDeviceSynchronize();
    if (err != cudaSuccess) {
        std::cerr << "Could not construct sphere on device" << std::endl;
        std::cerr << "CUDA error: " << cudaGetErrorString(err) << std::endl;
        return nullptr;
    }

    allocated_hittables.push_back(d_sphere);
    return d_sphere;
}

__global__ void cu_allocate_list(cu_hittable*** d_objects, int d_num_objects,
                                 cu_hittable** hittable_list_ptr) {
    cu_hittable_list* new_ptr = new cu_hittable_list();
    new_ptr->set_objects(d_objects, d_num_objects);
    *hittable_list_ptr = new_ptr;
}

__global__ void render(cu_hittable** d_world, cu_camera* d_cam,
                       color3* d_output) {
    d_cam->initRandState();

    int width = d_cam->image_width;
    int height = d_cam->image_height;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            ray ray = d_cam->get_ray(i, j);
            // printf("Ray: %f, %f, %f\n", ray.direction().x(),
            // ray.direction().y(),
            //        ray.direction().z());
            printf("%d, %d\n", i, j);
            color3 op = d_cam->ray_color_iter(ray, *d_world, d_cam->max_depth);
            d_output[i * width + j] = op;
        }
    }
}

__global__ void render_parallel(cu_hittable** d_world, cu_camera* d_cam,
                                color3* d_output) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    int width = d_cam->image_width;
    int height = d_cam->image_height;

    // printf("DIM: %d, %d\n", x, y);

    if (x >= height || y >= width) return;

    color3 op = color3(0, 0, 0);
    for (int i = 0; i < d_cam->samples_per_pixel; i++) {
        auto ray = d_cam->get_ray(y, x);
        op += d_cam->ray_color_iter(ray, *d_world, d_cam->max_depth);
    }

    op *= d_cam->pixel_samples_scale;
    d_output[x * width + y] = op;
}

void Allocator::test(cu_camera cam, color3* output) {
    std::clog << "Hello Render" << std::endl;
    cu_camera* d_cam;

    cudaError_t err = cudaMallocManaged(&d_cam, sizeof(cu_camera));
    if (err != cudaSuccess) {
        std::clog << "Could not allocate camera on the GPU" << std::endl;
        return;
    }

    *d_cam = cam;

    color3* d_output;
    err = cudaMallocManaged(
        &d_output, cam.image_width * cam.image_height * sizeof(color3));
    if (err != cudaSuccess) {
        std::clog << "Could not allocate camera on the GPU" << std::endl;
    }

    dim3 threads_per_block(16, 16);  // 16x16 threads per block
    dim3 number_of_blocks(ceil(cam.image_height / 16.0),
                          ceil(cam.image_width / 16.0));

    render_parallel<<<number_of_blocks, threads_per_block>>>(world, d_cam,
                                                             d_output);
    // render<<<1, 1>>>(world, d_cam, d_output);
    err = cudaDeviceSynchronize();

    if (err != cudaSuccess) {
        std::cerr << "CUDA error: " << cudaGetErrorString(err) << std::endl;
        return;
    }

    for (int i = 0; i < cam.image_height * cam.image_width; i++) {
        // std::clog<<d_output[i]<<std::endl;
        output[i] = d_output[i];
    }

    cudaFree(d_cam);
    cudaFree(d_output);
}

cu_hittable** Allocator::allocate_list() {
    cu_hittable** d_hittable_list;
    auto err = cudaMallocManaged(&d_hittable_list, sizeof(cu_hittable*));
    if (err != cudaSuccess) {
        std::cerr << "Could not allocate hittable_list ::cudaMalloc failed"
                  << std::endl;
        std::cerr << "CUDA error: " << cudaGetErrorString(err) << std::endl;
        return nullptr;
    }

    cu_hittable*** d_objects;
    cudaMallocManaged(&d_objects,
                      allocated_hittables.size() * sizeof(cu_hittable**));

    for (int i = 0; i < allocated_hittables.size(); i++) {
        d_objects[i] = allocated_hittables[i];
    }

    cu_allocate_list<<<1, 1>>>(d_objects, allocated_hittables.size(),
                               d_hittable_list);

    err = cudaDeviceSynchronize();
    if (err != cudaSuccess) {
        std::cerr << "Could not construct hittable_list" << std::endl;
        std::cerr << "CUDA error: " << cudaGetErrorString(err) << std::endl;
        return nullptr;
    }

    this->world = d_hittable_list;
    return d_hittable_list;
}

__global__ void cu_allocate_lambertian(color3* albedo,
                                       cu_material** material_ptr) {
    *material_ptr = new cu_lambertian(*albedo);
}

cu_material** Allocator::allocate_lambertian(color3 albedo) {
    cu_material** lambertian_ptr;

    auto err = cudaMallocManaged(&lambertian_ptr, sizeof(cu_material*));
    if (err != cudaSuccess) {
        std::clog
            << "Could not allocate material:lambertian ::cudaMalloc failed"
            << std::endl;
        std::cerr << "CUDA error: " << cudaGetErrorString(err) << std::endl;
        return nullptr;
    }

    color3* d_color;
    cudaMallocManaged(&d_color, sizeof(color3));
    *d_color = albedo;

    cu_allocate_lambertian<<<1, 1>>>(d_color, lambertian_ptr);

    err = cudaDeviceSynchronize();
    if (err != cudaSuccess) {
        std::cerr << "Could not construct material:lambertian" << std::endl;
        std::cerr << "CUDA error: " << cudaGetErrorString(err) << std::endl;
        return nullptr;
    }

    allocated_materials.push_back(lambertian_ptr);
    return lambertian_ptr;
}

__global__ void cu_allocate_metal(color3* albedo, double fuzz,
                                  cu_material** material_ptr) {
    *material_ptr = new cu_metal(*albedo, fuzz);
}

cu_material** Allocator::allocate_metal(const color3 albedo, double fuzz) {
    cu_material** metal_ptr;

    auto err = cudaMallocManaged(&metal_ptr, sizeof(cu_material*));
    if (err != cudaSuccess) {
        std::cerr << "Could not allocate item::cudaMalloc failed" << std::endl;
        std::cerr << "CUDA error: " << cudaGetErrorString(err) << std::endl;
        return nullptr;
    }

    color3* d_color;
    cudaMallocManaged(&d_color, sizeof(color3));
    *d_color = albedo;

    cu_allocate_metal<<<1, 1>>>(d_color, fuzz, metal_ptr);

    err = cudaDeviceSynchronize();
    if (err != cudaSuccess) {
        std::cerr << "Could not allocate item" << std::endl;
        std::cerr << "CUDA error: " << cudaGetErrorString(err) << std::endl;
        return nullptr;
    }

    allocated_materials.push_back(metal_ptr);
    return metal_ptr;
}

__global__ void cu_allocate_dielectric(double refraction_index,
                                       cu_material** material_ptr) {
    *material_ptr = new cu_dielectric(refraction_index);
}

cu_material** Allocator::allocate_dielectric(double refraction_index) {
    cu_material** dielectric_ptr;

    auto err = cudaMallocManaged(&dielectric_ptr, sizeof(cu_material*));
    if (err != cudaSuccess) {
        std::cerr << "Could not allocate item::cudaMalloc failed" << std::endl;
        std::cerr << "CUDA error: " << cudaGetErrorString(err) << std::endl;
        return nullptr;
    }

    cu_allocate_dielectric<<<1, 1>>>(refraction_index, dielectric_ptr);

    err = cudaDeviceSynchronize();
    if (err != cudaSuccess) {
        std::cerr << "Could not allocate item" << std::endl;
        std::cerr << "CUDA error: " << cudaGetErrorString(err) << std::endl;
        return nullptr;
    }

    allocated_materials.push_back(dielectric_ptr);
    return dielectric_ptr;
}
