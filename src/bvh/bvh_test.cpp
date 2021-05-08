#include <gtest/gtest.h>
#include "boundable.h"
#include "bvh.hpp"
#include "ray.h"
#include "ray_tracing.h"

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

void assert_childbox_at_index_correct(const BBox& bbox, const int childIndex, const BBox& expectedBox){
    BBox actualChildBox;
    childBox_at_index(bbox, childIndex, actualChildBox);
    box_eq(expectedBox, actualChildBox);
}

TEST(Octree, calculate_childbox_at_index){
    BBox bBox = BBox(vec3(-1), vec3(1));
    assert_childbox_at_index_correct(bBox, 0, BBox(vec3(-1),vec3(0)));
    assert_childbox_at_index_correct(bBox, 2, BBox(vec3(-1,0,-1),vec3(0,1,0)));
    assert_childbox_at_index_correct(bBox, 6, BBox(vec3(-1,0,0),vec3(0,1,1)));
    assert_childbox_at_index_correct(bBox, 4, BBox(vec3(-1,-1,0),vec3(0,0,1)));
    assert_childbox_at_index_correct(bBox, 1, BBox(vec3(0,-1,-1),vec3(1,0,0)));
    assert_childbox_at_index_correct(bBox, 3, BBox(vec3(0,0,-1),vec3(1,1,0)));
    assert_childbox_at_index_correct(bBox, 7, BBox(vec3(0),vec3(1)));
    assert_childbox_at_index_correct(bBox, 5, BBox(vec3(0,-1,0),vec3(1,0,1)));
}

TEST(Octree, insert_object)
{
    Extent sphere1Extent;
    Sphere sphere1(vec3(3,3,3),1);
    Sphere sphere2(vec3(-3,-3,-3),2);
    Extent *sphere1Ext;
    Extent *sphere2Ext;
    vec3 origin(0);
    vec3 normal[] = {vec3(1,0,0), vec3(0,1,0), vec3(0,0,1)};

    sphere1.calculateBounds(normal, 3, origin, sphere1Ext);
    sphere2.calculateBounds(normal, 3, origin, sphere2Ext);
    double expectdFor1[][2] = {{2,4},{2,4},{2,4}};
    double expectdFor2[][2] = {{-5,-1},{-5,-1},{-5,-1}};
    assertBoundDistance(expectdFor1, sphere1Ext->d,3);
    assertBoundDistance(expectdFor2, sphere2Ext->d,3);


    Extent *sceneExtent = new Extent();
    sceneExtent->d[0][0]=-10;
    sceneExtent->d[0][1]=10;
    sceneExtent->d[1][0]=-10;
    sceneExtent->d[1][1]=10;
    sceneExtent->d[2][0]=-10;
    sceneExtent->d[2][1]=10;
  
    Octree tree(const_cast<const Extent*>(sceneExtent));
    tree.insert(tree.root, const_cast<const Extent*>(sphere1Ext), tree.bbox, 0);
    ASSERT_TRUE(tree.root->isLeaf);
    ASSERT_EQ(1,tree.root->nodeExtentsList.size());
    ASSERT_EQ(sphere1Ext, tree.root->nodeExtentsList[0]);
    ASSERT_EQ(nullptr,tree.root->child[0]);

    tree.insert(tree.root, const_cast<const Extent*>(sphere2Ext), tree.bbox, 0);
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

    ASSERT_EQ(sphere1Ext, tree.root->child[7]->nodeExtentsList[0]);
    ASSERT_EQ(sphere2Ext, tree.root->child[0]->nodeExtentsList[0]);

    vec3_eq(vec3(-10,-10,-10),tree.bbox.bounds[0]);
    vec3_eq(vec3(10,10,10),tree.bbox.bounds[1]);

    //insert a third sphere in the same child box as sphere1
    Sphere sphere3(vec3(2,2,2),0.2);
    Extent* sphere3Ext;
    sphere3.calculateBounds(normal, 3, origin, sphere3Ext);
    tree.insert(tree.root, const_cast<const Extent*>(sphere3Ext), tree.bbox, 0);
    ASSERT_EQ(0, tree.root->child[7]->nodeExtentsList.size());
    ASSERT_EQ(1, tree.root->child[7]->child[0]->child[7]->nodeExtentsList.size());
    ASSERT_EQ(sphere1Ext, tree.root->child[7]->child[0]->child[7]->nodeExtentsList[0]);
    ASSERT_EQ(sphere3Ext, tree.root->child[7]->child[0]->child[0]->nodeExtentsList[0]);
}

TEST(Octree, bottom_up_build){
    Extent sphere1Extent;
    vec3 origin(0);
    vec3 normal[] = {vec3(1,0,0), vec3(0,1,0), vec3(0,0,1)};

    Sphere sphere1(vec3(3,3,3),1);
    Sphere sphere2(vec3(-3,-3,-3),2);
    Sphere sphere3(vec3(2,2,2),0.2);
    Extent* sphere1Ext;
    Extent* sphere2Ext;
    Extent* sphere3Ext;
    sphere1.calculateBounds(normal, 3, origin, sphere1Ext);
    sphere2.calculateBounds(normal, 3, origin, sphere2Ext);
    sphere3.calculateBounds(normal, 3, origin, sphere3Ext);

    Extent* sceneExtent = new Extent();
    sceneExtent->d[0][0]=-10;
    sceneExtent->d[0][1]=10;
    sceneExtent->d[1][0]=-10;
    sceneExtent->d[1][1]=10;
    sceneExtent->d[2][0]=-10;
    sceneExtent->d[2][1]=10;
 
    Octree tree(const_cast<const Extent*>(sceneExtent));
    tree.insert(tree.root, const_cast<const Extent*>(sphere1Ext), tree.bbox, 0);
    tree.insert(tree.root, const_cast<const Extent*>(sphere2Ext), tree.bbox, 0);
    tree.insert(tree.root, const_cast<const Extent*>(sphere3Ext), tree.bbox, 0);

    tree.build(tree.root, tree.bbox);

    {
        Extent expectedBox;
        expectedBox.d[0][0] = 2;// {{2,5},{2,5},{2,5}};
        expectedBox.d[0][1] = 4;
        expectedBox.d[1][0] = 2;
        expectedBox.d[1][1] = 4;
        expectedBox.d[2][0] = 2;
        expectedBox.d[2][1] = 4;
        assertBoundDistance(expectedBox.d, tree.root->child[7]->child[0]->child[7]->currentNodeExtent->d, 3);
    }
    {
        Extent expectedBox;
        expectedBox.d[0][0] = -5;// {{2,5},{2,5},{2,5}};
        expectedBox.d[0][1] = -1;
        expectedBox.d[1][0] = -5;
        expectedBox.d[1][1] = -1;
        expectedBox.d[2][0] = -5;
        expectedBox.d[2][1] = -1;
        assertBoundDistance(expectedBox.d, tree.root->child[0]->currentNodeExtent->d, 3);
    }
    {
        //test the root box
        Extent expectedBox;
        expectedBox.d[0][0] = -5;
        expectedBox.d[0][1] = 4;
        expectedBox.d[1][0] = -5;
        expectedBox.d[1][1] = 4;
        expectedBox.d[2][0] = -5;
        expectedBox.d[2][1] = 4;
        assertBoundDistance(expectedBox.d, tree.root->currentNodeExtent->d, 3);
    }
    ASSERT_EQ(sphere1Ext, tree.root->child[7]->child[0]->child[7]->nodeExtentsList[0]);
    ASSERT_EQ(sphere3Ext, tree.root->child[7]->child[0]->child[0]->nodeExtentsList[0]);
}

TEST(bvh, create_BVH_1_object){
    std::vector<Sphere*> sceneObjects;
    sceneObjects.push_back(new Sphere(vec3(3,3,3), 1));
    BVH bvh(sceneObjects);
    ASSERT_TRUE(bvh.tree!=nullptr);
    ASSERT_EQ(1,bvh.tree->root->nodeExtentsList.size());
    ASSERT_EQ(true,bvh.tree->root->isLeaf);
    ASSERT_EQ(1,bvh.tree->root->nodeExtentsList[0]->object->r);
    vec3_eq(bvh.tree->root->nodeExtentsList[0]->object->center, vec3(3));

    hit_record hitRecord;

    const ray ray_N(vec3(0),vec3(1,0,0));
    Sphere *hitObject = nullptr;
    bool hitResult = bvh.intersect(ray_N, &hitObject, hitRecord);
    ASSERT_EQ(false, hitResult);

    const ray ray_E(vec3(0),vec3(0,1,0));
    hitResult = bvh.intersect(ray_E, &hitObject, hitRecord);
    ASSERT_EQ(false, hitResult);

    const ray ray_diagonal(vec3(0), vec3(sqrt(3)/3));
    hitResult = bvh.intersect(ray_diagonal, &hitObject, hitRecord);
    ASSERT_EQ(true, hitResult);
    sceneObjects.clear();
}


TEST(bvh, create_BVH_1_object_b){
    std::vector<Sphere*> sceneObjects;
    sceneObjects.push_back(new Sphere(vec3(3,0,0), 1));
    BVH bvh(sceneObjects);
    ASSERT_TRUE(bvh.tree!=nullptr);
    ASSERT_EQ(1,bvh.tree->root->nodeExtentsList.size());
    ASSERT_EQ(true,bvh.tree->root->isLeaf);
    ASSERT_EQ(1,bvh.tree->root->nodeExtentsList[0]->object->r);
    vec3_eq(bvh.tree->root->nodeExtentsList[0]->object->center, vec3(3,0,0));

    hit_record hitRecord;

    const ray ray_N(vec3(0),vec3(1,0,0));
    Sphere *hitObject = nullptr;
    bool hitResult = bvh.intersect(ray_N, &hitObject, hitRecord);
    ASSERT_EQ(true, hitResult);

    const ray ray_E(vec3(0),vec3(0,1,0));
    hitResult = bvh.intersect(ray_E, &hitObject, hitRecord);
    ASSERT_EQ(false, hitResult);

    const ray ray_diagonal(vec3(0), vec3(sqrt(3)/3));
    hitResult = bvh.intersect(ray_diagonal, &hitObject, hitRecord);
    ASSERT_EQ(false, hitResult);


    const ray ray_tangential(vec3(0), vec3(sqrt(8)/3,1/3,0));
    hitResult = bvh.intersect(ray_tangential, &hitObject, hitRecord);
    ASSERT_EQ(false, hitResult);
    
    sceneObjects.clear();
}

void assert_right_tile_indexes(int w, int h, int t_s, int id, int exp_sr, int exp_sc, int exp_er, int exp_ec){
    int startRow=-1;
    int startCol=-1;
    int endRow=-1;
    int endCol=-1;
    getTileIndexes(w, h, t_s, id, startRow, startCol, endRow, endCol);
    ASSERT_EQ(exp_sr, startRow)<<"id "<<id;
    ASSERT_EQ(exp_er, endRow)<<"id "<<id;
    ASSERT_EQ(exp_sc, startCol)<<"id "<<id;
    ASSERT_EQ(exp_ec, endCol)<<"id "<<id;
}

TEST(ray_tracing, tile_indexes_calculation){
    {
    int width = 16;
    int height = 16;
    int tileSize = 8;
    assert_right_tile_indexes(width, height, tileSize,0,0,0,8,8);
    assert_right_tile_indexes(width, height, tileSize,1,0,8,8,16);
    assert_right_tile_indexes(width, height, tileSize,2,8,0,16,8);
    assert_right_tile_indexes(width, height, tileSize,3,8,8,16,16);
    }
    {
    int width = 17;
    int height = 17;
    int tileSize = 8;
    assert_right_tile_indexes(width, height, tileSize,0,0,0,8,8);
    assert_right_tile_indexes(width, height, tileSize,1,0,8,8,16);
    assert_right_tile_indexes(width, height, tileSize,2,0,16,8,17);
    assert_right_tile_indexes(width, height, tileSize,3,8,0,16,8);
    assert_right_tile_indexes(width, height, tileSize,4,8,8,16,16);
    assert_right_tile_indexes(width, height, tileSize,5,8,16,16,17);
    assert_right_tile_indexes(width, height, tileSize,6,16,0,17,8);
    assert_right_tile_indexes(width, height, tileSize,7,16,8,17,16);
    assert_right_tile_indexes(width, height, tileSize,8,16,16,17,17);
    }
}

