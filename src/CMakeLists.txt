cmake_minimum_required (VERSION 3.11.0)
PROJECT(RayTracer C CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message(STATUS "Open MP package found")
else(OpenMP_CXX_FOUND)
    message(WARNING "Open MP package not found")
endif()

add_subdirectory("common")
add_subdirectory("bvh")
add_subdirectory("data_porting")


add_executable(tracer_marble_single_threaded  "marble_single_thread.cc")
target_link_libraries(tracer_marble_single_threaded tracer_common)
target_include_directories(tracer_marble_single_threaded PUBLIC "common")