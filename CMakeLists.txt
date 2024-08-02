cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(raytracer 
    VERSION 1.10
    LANGUAGES CXX CUDA
)

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CUDA_ARCHITECTURES 61)
set(CMAKE_BUILD_TYPE None)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build")
    set(CMAKE_CUDA_FLAGS ${CMAKE_CUDA_FLAGS} "-g -G")  # enable cuda-gdb
endif()

option(ENABLE_PROFILING "Enable profiling with gprof" OFF)

# Conditionally add profiling flags
if(ENABLE_PROFILING)
    message(STATUS "Profiling enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
else()
    message(STATUS "Profiling disabled")
endif()

file(GLOB_RECURSE src_files "${CMAKE_CURRENT_LIST_DIR}/src/*.[ch]pp")
file(GLOB_RECURSE include_files "${CMAKE_CURRENT_LIST_DIR}/include/*/*.[ch]pp")
file(GLOB_RECURSE kernel_files "${CMAKE_CURRENT_LIST_DIR}/kernel/*.cu")

add_executable(Main ${src_files} ${include_files} ${kernel_files})
set_target_properties(Main PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(Main PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(Main PUBLIC "${PROJECT_SOURCE_DIR}/kernel")
target_include_directories(Main PUBLIC "${PROJECT_BINARY_DIR}")
