cmake_minimum_required (VERSION 3.11.0)
PROJECT(RayTracer C CXX)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

add_subdirectory("common")
add_subdirectory("bvh")


add_executable(tracer_marble_single_threaded  "marble_single_thread.cc")
target_link_libraries(tracer_marble_single_threaded tracer_common)
target_include_directories(tracer_marble_single_threaded PUBLIC "common")