#pragma once

#include "cuda/cu_camera.hpp"
#include "cuda/cu_hittable_list.hpp"

void render(cu_camera& cam, color3* output, cu_hittable** world);

void run();
