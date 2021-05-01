#include <gtest/gtest.h>
#include "boundable.h"

TEST(Boundable_sphere, calculate_bounds1){
    Sphere *object = new Sphere(vec3(2,2,2), 1);
    Extent* outputExt;
    vec3 normal[] ={vec3(1,0,0),vec3(0,1,0),vec3(0,0,1)};
    object->calculateBounds(normal, 3, vec3(0), outputExt);
    ASSERT_DOUBLE_EQ(1,outputExt->d[0][0]);
    ASSERT_DOUBLE_EQ(3,outputExt->d[0][1]);
    ASSERT_DOUBLE_EQ(1,outputExt->d[1][0]);
    ASSERT_DOUBLE_EQ(3,outputExt->d[1][1]);
    ASSERT_DOUBLE_EQ(1,outputExt->d[2][0]);
    ASSERT_DOUBLE_EQ(3,outputExt->d[2][1]);
    delete object;
}

