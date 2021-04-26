include(FetchContent)

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  GIT_TAG v3.7.3)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

#target_link_libraries(foo PRIVATE nlohmann_json::nlohmann_json)

add_library(shapeio OBJECT "data_porting.cpp")
target_link_libraries(shapeio PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(shapeio PRIVATE "../common/")

add_executable(shapeio_main "data_main.cpp")
target_link_libraries(shapeio_main PRIVATE shapeio tracer_common)
target_include_directories(shapeio_main PRIVATE "../common/")
target_link_libraries(shapeio_main PRIVATE nlohmann_json::nlohmann_json)

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
)
target_include_directories(
    data_porting_test PRIVATE "../common"
)
include(GoogleTest)
gtest_discover_tests(data_porting_test)
