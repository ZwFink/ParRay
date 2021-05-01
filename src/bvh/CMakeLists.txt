add_library(bvhlib OBJECT  "boundable.cpp" "bvh.cpp")
target_include_directories(bvhlib PUBLIC "../common/")
find_package(OpenMP REQUIRED)
find_package(MPI REQUIRED)

add_executable(sphere_bvh_single_threaded "sphere_bvh.cpp")
add_compile_options(-O3 -g)
target_include_directories(sphere_bvh_single_threaded PUBLIC "../common/" "../data_porting" "./")
# target_link_libraries(sphere_bvh_single_threaded bvhlib tracer_common shapeio nlohmann_json::nlohmann_json OpenMP::OpenMP_CXX)
target_link_libraries(sphere_bvh_single_threaded bvhlib tracer_common nlohmann_json::nlohmann_json OpenMP::OpenMP_CXX)

if(OpenMP_CXX_FOUND)
  add_executable(bvh_mt "sphere_bvh_openmp.cpp")
  target_include_directories(bvh_mt PUBLIC "../common/" "../data_porting" "./")
  target_link_libraries(bvh_mt bvhlib tracer_common shapeio nlohmann_json::nlohmann_json OpenMP::OpenMP_CXX)
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
)
target_include_directories(
  bvh_test PUBLIC "../common")

include(GoogleTest)
gtest_discover_tests(bvh_test)

