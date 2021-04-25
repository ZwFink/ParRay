#include "bvh.hpp"
#include <stdio.h>
#include <stdint.h>
#include <algorithm> //for swap function
#include "boundable.h"
#include <queue>

BBox::BBox(vec3 min, vec3 max)
{
    bounds[0] = min;
    bounds[1] = max;
}

BBox &BBox::extendBy(const vec3 &p)
{
    if (p.x() < bounds[0].x())
        bounds[0].e[0] = p.x();
    if (p.y() < bounds[0].y())
        bounds[0].e[1] = p.y();
    if (p.z() < bounds[0].z())
        bounds[0].e[2] = p.z();
    if (p.x() > bounds[1].x())
        bounds[1].e[0] = p.x();
    if (p.y() > bounds[1].y())
        bounds[1].e[1] = p.y();
    if (p.z() > bounds[1].z())
        bounds[1].e[2] = p.z();
    return *this;
}

Octree::Octree(const Extent &sceneExtent)
{
    double xDiff = sceneExtent.d[0][1] - sceneExtent.d[0][0];
    double yDiff = sceneExtent.d[1][1] - sceneExtent.d[1][0];
    double zDiff = sceneExtent.d[2][1] - sceneExtent.d[2][0];
    double maxDiff = std::max(xDiff, std::max(yDiff, zDiff));
    vec3 minPlusMax(
        sceneExtent.d[0][0] + sceneExtent.d[0][1],
        sceneExtent.d[1][0] + sceneExtent.d[1][1],
        sceneExtent.d[2][0] + sceneExtent.d[2][1]);
    bbox.bounds[0] = (minPlusMax - maxDiff) * 0.5;
    bbox.bounds[1] = (minPlusMax + maxDiff) * 0.5;
    root = new OctreeNode;
}

void Octree::deleteOctreeNode(OctreeNode *&node)
{
    for (int i = 0; i < 8; i++)
    {
        if (node->child[i] != nullptr)
        {
            deleteOctreeNode(node->child[i]);
        }
    }
    delete node;
}

void calculateChildBox(const vec3 &objectCentroid, const BBox &nodeBox, BBox &childBox, int &childIndex)
{
    const vec3 nodeCentroid = nodeBox.centroid();
    int index = 1;
    for (uint8_t dim = 0; dim < 3; dim++)
    {
        if (objectCentroid.e[dim] > nodeCentroid.e[dim])
        {
            childIndex += index;
            //assign value to the child bounding box.
            childBox.bounds[0].e[dim] = nodeCentroid.e[dim];
            childBox.bounds[1].e[dim] = nodeBox.bounds[1].e[dim];
        }
        else
        {
            childBox.bounds[0].e[dim] = nodeBox.bounds[0].e[dim];
            childBox.bounds[1].e[dim] = nodeCentroid.e[dim];
        }
        index *= 2;
    }
}

void childBox_at_index(const BBox &parentBox, int childIndex, BBox &childBox)
{
    vec3 centroid = parentBox.centroid();
    int testIndex = 1;
    for (uint8_t dim = 0; dim < 3; dim++)
    {
        childBox.bounds[0].e[dim] = (childIndex & testIndex) ? centroid.e[dim] : parentBox.bounds[0].e[dim];
        childBox.bounds[1].e[dim] = (childIndex & testIndex) ? parentBox.bounds[1].e[dim] : centroid.e[dim];
        testIndex *= 2;
    }
}

void Octree::insert(const Extent *extent){
    insert(root, extent, bbox,0);
}

void Octree::insert(OctreeNode *&node, const Extent *extent, BBox &nodeBox, int depth)
{
    if (node->isLeaf)
    {
        //currently the maximum depth is hardcode at 16
        if (node->nodeExtentsList.size() == 0 || depth == 16)
        {
            node->nodeExtentsList.push_back(extent);
        }
        else
        {
            //Get those extents currently stored at the node and put them into the childrens
            node->isLeaf = false; // this will indicate the next recursion to assign the extent to its children
            while (node->nodeExtentsList.size())
            {
                insert(node, node->nodeExtentsList.back(), nodeBox, depth);
                node->nodeExtentsList.pop_back();
            }
            insert(node, extent, nodeBox, depth);
        }
    }
    else
    {
        //assign the input extent to one of the 8 children of the current node
        vec3 objectCentroid = extent->centroid();
        vec3 nodeCentroid = nodeBox.centroid();

        int childIndex = 0;
        BBox childBox;
        calculateChildBox(objectCentroid, nodeBox, childBox, childIndex);
        if (node->child[childIndex] == nullptr)
        {
            node->child[childIndex] = new OctreeNode;
        }
        insert(node->child[childIndex], extent, childBox, depth + 1);
    }
}

void Octree::build(OctreeNode *&node, const BBox &bbox)
{
    if (node->isLeaf)
    {
        for (const auto &e : node->nodeExtentsList)
        {
            node->currentNodeExtent.extendBy(*e);
        }
    }
    else
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            if (node->child[i] != nullptr)
            {
                BBox childBox;
                childBox_at_index(bbox, i, childBox);
                build(node->child[i], childBox);
                node->currentNodeExtent.extendBy(node->child[i]->currentNodeExtent);
            }
        }
    }
}

void Octree::build(){
    build(root, bbox);
}

const vec3 BVH::planeSetNormals[kNumPlaneSetNormals] = {
    vec3(1, 0, 0),
    vec3(0, 1, 0),
    vec3(0, 0, 1),
    vec3(sqrt(3) / 3.0, sqrt(3) / 3.0, sqrt(3) / 3.0),
    vec3(-sqrt(3) / 3.0, sqrt(3) / 3.0, sqrt(3) / 3.0),
    vec3(-sqrt(3) / 3.0, -sqrt(3) / 3.0, sqrt(3) / 3.0),
    vec3(sqrt(3) / 3.0, -sqrt(3) / 3.0, sqrt(3) / 3.0)
    };



BVH::BVH(std::vector<std::shared_ptr<Sphere>>& objects){
    Extent scene;
    extentList.reserve(objects.size());
    for (int i = 0; i < objects.size(); i++)
    {
        Extent objectExtent;
        objects[i]->calculateBounds(planeSetNormals, kNumPlaneSetNormals, vec3(0), objectExtent);
        scene.extendBy(objectExtent);
        objectExtent.object = objects[i];
        extentList[i] = objectExtent;
    }
    tree = new Octree(scene);

    for(int i=0; i<objects.size();i++){
        tree->insert(&extentList[i]);
    }

    tree->build();
}

BVH::~BVH(){
    delete tree;
}

bool BVH::intersect(const ray &ray, std::shared_ptr<Sphere> hit_object, hit_record& hit_record_out){
    double tHit = DBL_MAX;
    std::shared_ptr<Sphere> hitObject(nullptr);
    hit_record hitRecord;
    //common R*O and R*N results that can be used
    double n_dot_o[kNumPlaneSetNormals];
    double n_dot_r[kNumPlaneSetNormals];
    for(int i=0; i<kNumPlaneSetNormals; i++){
        n_dot_o[i] = dot(planeSetNormals[i], ray.origin());
        n_dot_r[i] = dot(planeSetNormals[i], ray.direction());
    }

    //first determine if the ray hit the root of octree
    double tNear = 0, tFar = DBL_MAX;
    int plane_index=-1;
    if(!tree->root->currentNodeExtent.interset(n_dot_o, n_dot_r, tNear, tFar, plane_index) || tFar<0){
        return false;
    }

    //find out the all extents that intersect the ray, and get the one with smallest hit time
    tHit = tFar;
    std::priority_queue<QueueElement> queue;
    queue.push(QueueElement(tree->root,0));
    int count = 0;
    while(!queue.empty() && queue.top().t<tHit){
        const OctreeNode *node = queue.top().node;
        queue.pop();
        if(node->isLeaf){
            for(const auto& e: node->nodeExtentsList){
                hit_record currentHitRecord;
                if(e->object->hit(ray, tNear, tHit, currentHitRecord) 
                && currentHitRecord.t <tHit){
                    hitObject = e->object;
                    hitRecord = currentHitRecord;
                    tHit = hitRecord.t;
                }
            }
        }
        else{
            for(int i=0;i<8;i++){
                if(node->child[i]!=nullptr){
                    double tNearChild = 0;
                    double tFarChild = tFar;
                    int planeIndex;
                    if(node->child[i]->currentNodeExtent.interset(n_dot_o, n_dot_r, tNearChild, tFarChild, planeIndex)){
                        double t = (tNearChild < 0 && tFarChild >=0)?tFarChild:tNearChild;
                        queue.push(QueueElement(node->child[i],t));
                    }
                }
            }
        }
    }

    if(hitObject!=nullptr){
        hit_object = hitObject;
        hit_record_out = hitRecord;
        return true;
    }
    return false;
}