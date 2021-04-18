cmake_minimum_required (VERSION 3.10.0)
PROJECT(RayTracer C CXX)
add_subdirectory("common")
add_subdirectory("bvh")

add_executable(tracer_marble_single_threaded  "marble_single_thread.cc")
target_include_directories(tracer_marble_single_threaded PUBLIC "common")