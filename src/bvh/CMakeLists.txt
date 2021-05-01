find_package(OpenMP REQUIRED)
find_package(MPI REQUIRED)

add_compile_options(-O3 -g)


if(OpenMP_CXX_FOUND)

# the bvh ray tracing library, including the ray tracing methods.
  add_library(bvhlib OBJECT "ray_tracing.cpp" "boundable.cpp" "bvh.cpp")
  target_include_directories(bvhlib PUBLIC "../common/")
  target_link_libraries(bvhlib tracer_common OpenMP::OpenMP_CXX )

# an executable for multiple thread bvh
  add_executable(bvh_mt "sphere_bvh_openmp.cpp")
  target_include_directories(bvh_mt PUBLIC "../common/" "../data_porting" "./")
  target_link_libraries(bvh_mt bvhlib tracer_common shapeio nlohmann_json::nlohmann_json OpenMP::OpenMP_CXX)

# an executable for single thread bvh
  add_executable(sphere_bvh_single "sphere_bvh_single.cpp")
  target_include_directories(sphere_bvh_single PUBLIC "../common/" "../data_porting" "./")
  target_link_libraries(sphere_bvh_single bvhlib tracer_common shapeio nlohmann_json::nlohmann_json OpenMP::OpenMP_CXX)
endif()

if(MPI_FOUND)
  add_executable(bvh_mpi "sphere_bvh_mpi.cpp")
  target_include_directories(bvh_mpi PUBLIC "../common/" "../data_porting" "./")
  include_directories(SYSTEM ${MPI_INCLUDE_PATH})
  target_link_libraries(bvh_mpi bvhlib tracer_common shapeio nlohmann_json::nlohmann_json OpenMP::OpenMP_CXX ${MPI_CXX_LIBRARIES})
endif()


enable_testing()
add_executable(
  bvh_test 
  bvh_test.cpp
  boundable_test.cpp
)
target_link_libraries(
  bvh_test
  gtest_main
  bvhlib
  tracer_common
)
target_include_directories(
  bvh_test PUBLIC "../common")
include(GoogleTest)
gtest_discover_tests(bvh_test)

