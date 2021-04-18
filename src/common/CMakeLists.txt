
set(tracer_common_source 
    "common.h"
    "ray.h"
    "vec3.h"
    "color.h"
    "hittable.h"
    "hittable_list.h"
    "sphere.h"
    "camera.h"
    "material.h")
add_library(tracer_common INTERFACE ${tracer_common_source})
target_include_directories(tracer_common INTERFACE ".")
