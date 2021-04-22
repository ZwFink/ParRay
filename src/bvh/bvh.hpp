#ifndef __H_BVH
#define __H_BVH

#include "vec3.h"
#include <float.h>
#include <vector>

const int kNumPlaneSetNormals = 7;


class BBox 
{ 
public: 
    BBox() {} 
    BBox(vec3 min, vec3 max);
    BBox& extendBy(const vec3& p);
    vec3 centroid() const { return (bounds[0] + bounds[1]) * 0.5; } 
    vec3& operator [] (bool i) { return bounds[i]; } 
    const vec3 operator [] (bool i) const { return bounds[i]; } 
    vec3 bounds[2] = {vec3(DBL_MIN), vec3(DBL_MAX)}; 
}; 

class Extent
{
public:
    Extent();
    void extendBy(const Extent &extents);
    bool interset(const double *numberator, const double *denominator, double &tNear, double &tFar, int &planeIndex);
    vec3 centroid() const;

public:
    double d[kNumPlaneSetNormals][2]; //the distance d values for each plane set normals
};

struct OctreeNode
{
    OctreeNode *child[8] = {nullptr};
    std::vector<const Extent *> nodeExtentsList; // pointer to the objects extents
    Extent currentNodeExtent;                    // extent of the octree node itself
    bool isLeaf = true;
};

struct QueueElement 
{ 
            const OctreeNode *node; // octree node held by this element in the queue 
            double t; // distance from the ray origin to the extents of the node 
            QueueElement(const OctreeNode *n, double tn) : node(n), t(tn) {} 
            // priority_queue behaves like a min-heap
            friend bool operator < (const QueueElement &a, const QueueElement &b) { return a.t > b.t; } 
};

class Octree
{
public:
    Octree(const Extent &sceneExtent);
    void insert(OctreeNode*& node, const Extent* extents, BBox &nodeBox, int depth);
    OctreeNode *root = nullptr; // make unique son don't have to manage deallocation
    BBox bbox;
private:
    void deleteOctreeNode(OctreeNode *&node);
};

void calculateChildBox(const vec3 &objectCentroid, const BBox &nodeBox, BBox &childBox, int &childIndex);
#endif

