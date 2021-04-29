find_package(benchmark REQUIRED)
if(benchmark_FOUND)
    message(STATUS "google benchmark found.")
else(benchmark_FOUND)
    message(WARNING "google benchmark NOT found.")
endif(benchmark_FOUND)


add_executable(bench_mark_sample "benchmark_test.cpp")
target_link_libraries(bench_mark_sample PRIVATE benchmark pthread)

