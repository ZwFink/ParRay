#ifndef __H_BVH
#define __H_BVH

#include "vec3.h"
#include <float.h>
#include <vector>
#include "ray.h"
#include "boundable.h"
#include "hittable.h"


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
    void insert(const Extent * extent);
    void insert(OctreeNode*& node, const Extent* extents, BBox &nodeBox, int depth);
    OctreeNode *root = nullptr; // make unique son don't have to manage deallocation
    void build(OctreeNode*& node, const BBox &bbox);
    void build();
    BBox bbox;
private:
    void deleteOctreeNode(OctreeNode *&node);
};

class BVH{
    public:
    BVH(std::vector<std::shared_ptr<Sphere>>& scene);
    ~BVH();
    bool intersect(const ray &ray, std::shared_ptr<Sphere> hit_object, hit_record &hitRecord);
    Octree *tree = nullptr; 
    private:
    static const vec3 planeSetNormals[kNumPlaneSetNormals];
    std::vector<std::shared_ptr<Extent>> extentList;
};

/**
 * @brief Divide the node box into 8 child boxes. Assign the object to one of them based on the relative
 * positions of node box centroid and object centroid. 
 * @param[out] childIndex reference to output childIndex corresponding to the assigned childBox
 * @param[out] childBox reference to output childBox
 * @param[in] objectCentroid the object centroid
 * @param[in] nodeBox the bounding box of the current node
 */
void calculateChildBox(const vec3 &objectCentroid, const BBox &nodeBox, BBox &childBox, int &childIndex);

/**
 * @brief Calculate the child bounding box at child index
 * @param[in] parentBox the parent bounding box
 * @param[in] childIndex the child box index from 0-7
 * @param[out] childBox the child box
 */
void childBox_at_index(const BBox &parentBox, int childIndex, BBox &childBox);
#endif

