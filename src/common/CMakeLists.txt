
set(tracer_common_source 
    "common.h"
    "color.h"
    "hittable_list.h"
    "sphere.h"
    "camera.h"
    "material.h")
add_library(tracer_common OBJECT ${tracer_common_source})
target_include_directories(tracer_common PUBLIC ".")
