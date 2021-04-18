
add_executable(bvhHello "bvh.cpp")
target_include_directories(bvhHello PUBLIC "../common")
target_link_libraries(bvhHello tracer_common)

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

