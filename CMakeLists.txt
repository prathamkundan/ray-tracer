cmake_minimum_required(VERSION 3.10)

project(raytracer VERSION 1.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE src_files "${CMAKE_CURRENT_LIST_DIR}/src/*.[ch]pp")
file(GLOB_RECURSE include_files "${CMAKE_CURRENT_LIST_DIR}/src/*.[ch]pp")

add_executable(Main ${src_files} ${include_files})

add_subdirectory(${PROJECT_SOURCE_DIR}/deps/glm)

target_link_libraries(Main glm)

target_include_directories(Main PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(Main PUBLIC "${PROJECT_BINARY_DIR}")
