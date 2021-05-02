add_executable(tracer_marble_single_threaded  "marble_single_thread.cc")
target_link_libraries(tracer_marble_single_threaded PRIVATE tracer_common PRIVATE OpenMP::OpenMP_CXX)
target_include_directories(tracer_marble_single_threaded PUBLIC "../common")
