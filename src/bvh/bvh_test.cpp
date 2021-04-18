#include <gtest/gtest.h>

TEST(SanityCheck, testcase1)
{
    int a = 3;
    int b = 3;
    ASSERT_EQ(a,b);
}