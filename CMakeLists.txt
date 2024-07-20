cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(raytracer VERSION 1.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
file(GLOB_RECURSE include_files "${CMAKE_CURRENT_LIST_DIR}/src/*.[ch]pp")

add_executable(Main ${src_files} ${include_files})

add_subdirectory(${PROJECT_SOURCE_DIR}/deps/glm)

target_link_libraries(Main glm)

target_include_directories(Main PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(Main PUBLIC "${PROJECT_BINARY_DIR}")
