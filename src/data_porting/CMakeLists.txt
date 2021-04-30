 include(FetchContent)
 
 FetchContent_Declare(json
   GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
   GIT_TAG v3.7.3)
 
 FetchContent_GetProperties(json)
 if(NOT json_POPULATED)
   FetchContent_Populate(json)
   add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
 endif()
 
 
 add_library(shapeio OBJECT "data_porting.cpp")
 target_link_libraries(shapeio PRIVATE nlohmann_json::nlohmann_json bvhlib)
 target_include_directories(shapeio PRIVATE "../common/" "../bvh")
 
add_executable(shapeio_main "data_main.cpp")
target_link_libraries(shapeio_main PRIVATE shapeio tracer_common bvhlib)
target_include_directories(shapeio_main PRIVATE "../common/" "../bvh")
 target_link_libraries(shapeio_main PRIVATE nlohmann_json::nlohmann_json)
 
 add_executable(gen_random_scene "sphere_gen.cpp")
 target_link_libraries(gen_random_scene  PRIVATE shapeio tracer_common bvhlib)
 target_include_directories(gen_random_scene  PRIVATE "../common/" "../bvh")
 target_link_libraries(gen_random_scene  PRIVATE nlohmann_json::nlohmann_json)
 
 
 enable_testing()
 add_executable(
     data_porting_test
     data_porting_test.cpp
 )
 target_link_libraries(
     data_porting_test
     gtest_main
     shapeio
     tracer_common
     nlohmann_json::nlohmann_json
      bvhlib
 )
 target_include_directories(
     data_porting_test PRIVATE "../common"
     "../bvh"
 )
 include(GoogleTest)
 gtest_discover_tests(data_porting_test)
