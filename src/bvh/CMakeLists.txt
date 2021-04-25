add_library(bvhlib OBJECT  "boundable.cpp" "bvh.cpp")
target_include_directories(bvhlib PUBLIC "../common/")

add_executable(sphere_bvh_single_threaded "sphere_bvh.cpp")
target_include_directories(sphere_bvh_single_threaded PUBLIC "../common/")
target_link_libraries(sphere_bvh_single_threaded bvhlib tracer_common)


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

