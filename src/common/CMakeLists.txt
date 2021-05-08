add_library(tracer_common OBJECT "vec3.cpp" "sphere.cpp" "color.cpp" "common.cpp" "hittable_list.cpp")
target_include_directories(tracer_common PUBLIC ".")
