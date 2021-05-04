find_package(benchmark REQUIRED)
find_package(MPI REQUIRED)

if(benchmark_FOUND)
    message(STATUS "google benchmark found.")

    add_executable(bench_mark_sample "benchmark_test.cpp")
    target_link_libraries(bench_mark_sample PRIVATE benchmark pthread)

    add_executable(bm_ray_tracing "benchmark_bvh.cpp")
#    target_link_libraries(bm_ray_tracing PRIVATE bvhlib shapeio benchmark pthread)
    target_link_libraries(bm_ray_tracing bvhlib tracer_common shapeio nlohmann_json::nlohmann_json OpenMP::OpenMP_CXX benchmark pthread)
    target_include_directories(bm_ray_tracing PUBLIC "../common/" "../bvh" "../data_porting")

    add_executable(bm_mpi "benchmark_bvh_mpi.cpp")
    target_include_directories(bm_mpi PUBLIC "../common/" "../data_porting" "../bvh")
    include_directories(SYSTEM ${MPI_INCLUDE_PATH})
    target_link_libraries(bm_mpi bvhlib tracer_common shapeio nlohmann_json::nlohmann_json OpenMP::OpenMP_CXX ${MPI_CXX_LIBRARIES} benchmark pthread)
else(benchmark_FOUND)
    message(WARNING "google benchmark NOT found.")
endif(benchmark_FOUND)

