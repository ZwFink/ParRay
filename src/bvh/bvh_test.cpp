#include <gtest/gtest.h>
#include "bvh.hpp"

TEST(SanityCheck, testcase1)
{
    int a = 3;
    int b = 3;
    ASSERT_EQ(a,b);
}

TEST(Extent, create_extent){
    Extent *a = new Extent();
}