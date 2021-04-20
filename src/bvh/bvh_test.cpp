#include <gtest/gtest.h>
#include "bvh.hpp"

TEST(SanityCheck, testcase1)
{
    int a = 3;
    int b = 3;
    ASSERT_EQ(a, b);
}

TEST(Extent, create_extent)
{
    Extent *a = new Extent();
    for (int i = 0; i < 7; i++)
    {
        ASSERT_TRUE(a->d[i][0] > 999999);
        ASSERT_TRUE(a->d[i][1] < -999999);
    }
}

void vec3_eq(const vec3 &a, const vec3 &b)
{
    for (int i = 0; i < 3; i++)
    {
        ASSERT_DOUBLE_EQ(a.e[i], b.e[i]) << "vec3 difference at dimension " << i;
    }
}

TEST(Extent, centroid_of_extent)
{
    Extent *a = new Extent();
    double d[7][2];
    a->d[0][0] = 5, a->d[0][1] = 10;
    a->d[1][0] = 3, a->d[1][1] = 6;
    a->d[2][0] = 8, a->d[2][1] = 9;
    auto expected_c = vec3(7.5, 4.5, 8.5);
    delete a;
}

void box_eq(const BBox &a, const BBox &b)
{
    vec3_eq(a.bounds[0], b.bounds[0]);
    vec3_eq(a.bounds[1], b.bounds[1]);
}

TEST(BBox, centoid_calcualtion)
{
    BBox nodeBox = BBox(vec3(-1), vec3(1));
    vec3_eq(vec3(0), nodeBox.centroid());
}

void assert_calculate_childbox_right(const vec3 &obj_centroid, const BBox &node_box, const int expected_child_index, const BBox &expected_child_box)
{
    int childIndex=0;
    BBox actual_childbox;
    calculateChildBox(obj_centroid, node_box, actual_childbox, childIndex);
    ASSERT_EQ(expected_child_index, childIndex);
    box_eq(expected_child_box, actual_childbox);
}

TEST(OctreeNode, calculate_child_box)
{
    BBox nodeBox = BBox(vec3(-1), vec3(1));
    assert_calculate_childbox_right(vec3(0.001), nodeBox, 7, BBox(vec3(0), vec3(1)));
    assert_calculate_childbox_right(vec3(-0.001), nodeBox, 0, BBox(vec3(-1), vec3(0)));
    assert_calculate_childbox_right(vec3(0.001,0.001,-0.001), nodeBox, 3, BBox(vec3(0,0,-1), vec3(1,1,0)));
    assert_calculate_childbox_right(vec3(0.001,-0.001,-0.001), nodeBox, 1, BBox(vec3(0,-1,-1), vec3(1,0,0)));
    assert_calculate_childbox_right(vec3(0.001,-0.001,0.001), nodeBox, 5, BBox(vec3(0,-1,0), vec3(1,0,1)));
}