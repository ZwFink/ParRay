cmake_minimum_required (VERSION 3.13.0)
PROJECT(RayTracer C CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

link_directories("/home/zanef2/ceesd/miniforge3/envs/ceesd/lib")


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

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message(STATUS "Open MP package found")
else(OpenMP_CXX_FOUND)
    message(WARNING "Open MP package not found")
endif()

add_subdirectory("common")
add_subdirectory("bvh")
add_subdirectory("data_porting")
add_subdirectory("benchmark")
add_subdirectory("simple_tracer")

