#include <gtest/gtest.h>
#include "boundable.h"
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

void assertBoundDistance(double expect[][2], double actual[][2], int size){
   for(int i=0; i<size;i++) {
       ASSERT_DOUBLE_EQ(expect[i][0],actual[i][0])<<" The distance vector["<<i<<"][0] is different";
       ASSERT_DOUBLE_EQ(expect[i][1],actual[i][1])<<" The distance vector["<<i<<"][1] is different";
   }
}

template<typename T>
void assertArraysEqual(T* arrayA, T* arrayB, int size){
    for(int i=0;i<size;i++){
       ASSERT_EQ(arrayA[i], arrayB[i])<<"Element "<<i<<" is different";
    }
}



TEST(Octree, insert_object)
{
    Extent sphere1Extent;
    Sphere sphere1(vec3(3,3,3),1);
    Sphere sphere2(vec3(-3,-3,-3),2);
    Extent sphere1Ext;
    Extent sphere2Ext;
    vec3 origin(0);
    vec3 normal[] = {vec3(1,0,0), vec3(0,1,0), vec3(0,0,1)};

    sphere1.calculateBounds(normal, 3, origin, sphere1Ext);
    sphere2.calculateBounds(normal, 3, origin, sphere2Ext);
    double expectdFor1[][2] = {{2,4},{2,4},{2,4}};
    double expectdFor2[][2] = {{-5,-1},{-5,-1},{-5,-1}};
    assertBoundDistance(expectdFor1, sphere1Ext.d,3);
    assertBoundDistance(expectdFor2, sphere2Ext.d,3);


    Extent sceneExtent;
    sceneExtent.d[0][0]=-10;
    sceneExtent.d[0][1]=10;
    sceneExtent.d[1][0]=-10;
    sceneExtent.d[1][1]=10;
    sceneExtent.d[2][0]=-10;
    sceneExtent.d[2][1]=10;
  
    Octree tree(sceneExtent);
    tree.insert(tree.root, &sphere1Ext, tree.bbox, 0);
    ASSERT_TRUE(tree.root->isLeaf);
    ASSERT_EQ(1,tree.root->nodeExtentsList.size());
    ASSERT_EQ(&sphere1Ext, tree.root->nodeExtentsList[0]);
    ASSERT_EQ(nullptr,tree.root->child[0]);

    tree.insert(tree.root, &sphere2Ext, tree.bbox, 0);
    ASSERT_FALSE(tree.root->isLeaf);
    ASSERT_EQ(0, tree.root->nodeExtentsList.size());

    OctreeNode *expected[7]={nullptr};
//    assertArraysEqual<OctreeNode*>(expected, tree.root->child,7);
    ASSERT_NE(nullptr, tree.root->child[0]);
    ASSERT_EQ(nullptr, tree.root->child[1]);
    ASSERT_EQ(nullptr, tree.root->child[2]);
    ASSERT_EQ(nullptr, tree.root->child[3]);
    ASSERT_EQ(nullptr, tree.root->child[4]);
    ASSERT_EQ(nullptr, tree.root->child[5]);
    ASSERT_EQ(nullptr, tree.root->child[6]);
    ASSERT_NE(nullptr, tree.root->child[7]);

    ASSERT_EQ(&sphere1Ext, tree.root->child[7]->nodeExtentsList[0]);
    ASSERT_EQ(&sphere2Ext, tree.root->child[0]->nodeExtentsList[0]);

    vec3_eq(vec3(-10,-10,-10),tree.bbox.bounds[0]);
    vec3_eq(vec3(10,10,10),tree.bbox.bounds[1]);

    //insert a third sphere in the same child box as sphere1
    Sphere sphere3(vec3(2,2,2),0.2);
    Extent sphere3Ext;
    sphere3.calculateBounds(normal, 3, origin, sphere3Ext);
    tree.insert(tree.root, &sphere3Ext, tree.bbox, 0);
    ASSERT_EQ(0, tree.root->child[7]->nodeExtentsList.size());
    ASSERT_EQ(1, tree.root->child[7]->child[0]->child[7]->nodeExtentsList.size());
    ASSERT_EQ(&sphere1Ext, tree.root->child[7]->child[0]->child[7]->nodeExtentsList[0]);
    ASSERT_EQ(&sphere3Ext, tree.root->child[7]->child[0]->child[0]->nodeExtentsList[0]);
}