
add_executable(bvhHello "bvh.cpp")

enable_testing()
add_executable(
  bvh_test 
  bvh_test.cpp
)
target_link_libraries(
  bvh_test
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(bvh_test)

