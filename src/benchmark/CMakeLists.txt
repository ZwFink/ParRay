find_package(benchmark REQUIRED)
if(benchmark_FOUND)
    message(STATUS "google benchmark found.")

    add_executable(bench_mark_sample "benchmark_test.cpp")
    target_link_libraries(bench_mark_sample PRIVATE benchmark pthread)

    add_executable(bm_ray_tracing "benchmark_bvh.cpp")
#    target_link_libraries(bm_ray_tracing PRIVATE bvhlib shapeio benchmark pthread)
    target_link_libraries(bm_ray_tracing bvhlib tracer_common shapeio nlohmann_json::nlohmann_json OpenMP::OpenMP_CXX benchmark pthread)
    target_include_directories(bm_ray_tracing PUBLIC "../common/" "../bvh" "../data_porting")
else(benchmark_FOUND)
    message(WARNING "google benchmark NOT found.")
endif(benchmark_FOUND)

