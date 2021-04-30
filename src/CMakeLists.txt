cmake_minimum_required (VERSION 3.13.0)
PROJECT(RayTracer C CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(OpenMP REQUIRED)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g")

add_subdirectory("common")
add_subdirectory("bvh")
add_subdirectory("data_porting")


add_executable(tracer_marble_single_threaded  "marble_single_thread.cc")
target_link_libraries(tracer_marble_single_threaded PRIVATE tracer_common PRIVATE OpenMP::OpenMP_CXX)
target_include_directories(tracer_marble_single_threaded PUBLIC "common")
