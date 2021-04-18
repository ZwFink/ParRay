add_library(bvhlib OBJECT "bvh.cpp")
target_include_directories(bvhlib PUBLIC "../common/")

enable_testing()
add_executable(
  bvh_test 
  bvh_test.cpp
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

