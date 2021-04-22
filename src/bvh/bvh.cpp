#include "bvh.hpp"
#include <stdio.h>
#include <stdint.h>
#include <algorithm> //for swap function
#include "boundable.h"

BBox::BBox(vec3 min, vec3 max){
    bounds[0]=min;
    bounds[1]=max;
}

BBox& BBox::extendBy(const vec3& p){
        if (p.x() < bounds[0].x()) bounds[0].e[0] = p.x(); 
        if (p.y() < bounds[0].y()) bounds[0].e[1] = p.y(); 
        if (p.z() < bounds[0].z()) bounds[0].e[2] = p.z(); 
        if (p.x() > bounds[1].x()) bounds[1].e[0] = p.x(); 
        if (p.y() > bounds[1].y()) bounds[1].e[1] = p.y(); 
        if (p.z() > bounds[1].z()) bounds[1].e[2] = p.z(); 
        return *this; 
}

Extent::Extent()
{
    for (uint8_t i = 0; i < kNumPlaneSetNormals; ++i)
    {
        d[i][0] = DBL_MAX;
        d[i][1] = -DBL_MAX;
    }
}

void Extent::extendBy(const Extent &extents)
{
    for (int i = 0; i < kNumPlaneSetNormals; ++i)
    {
        if (extents.d[i][0] < d[i][0])
            d[i][0] = extents.d[i][0];
        if (extents.d[i][1] > d[i][1])
            d[i][1] = extents.d[i][1];
    }
}

bool Extent::interset(const double *numberator, const double *denominator, double &tNear, double &tFar, int &planeIndex)
{
    for (int i = 0; i < kNumPlaneSetNormals; i++)
    {
        double tn = (d[i][0] - numberator[i]) / denominator[i];
        double tf = (d[i][1] - numberator[i]) / denominator[i];
        if (denominator[i] < 0)
            std::swap(tn, tf);
        if (tn > tNear)
            tNear = tn, planeIndex = i;
        if (tf < tFar)
            tFar = tf;
        if (tNear > tFar)
            return false;
    }
    return true;
}

vec3 Extent::centroid() const {
    return vec3(
        (d[0][0]+d[0][1])*0.5,
        (d[1][0]+d[1][1])*0.5,
        (d[2][0]+d[2][1])*0.5
    );
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

void Octree::deleteOctreeNode(OctreeNode*&node){
     for (int i = 0; i < 8; i++) { 
                if (node->child[i] != nullptr) { 
                    deleteOctreeNode(node->child[i]); 
                } 
            } 
      delete node; 
}


/**
 * @brief Divide the node box into 8 child boxes. Assign the object to one of them based on the relative
 * positions of node box centroid and object centroid. 
 * @param[out] childIndex reference to output childIndex corresponding to the assigned childBox
 * @param[out] childBox reference to output childBox
 * @param[in] objectCentroid the object centroid
 * @param[in] nodeBox the bounding box of the current node
 */
void calculateChildBox(const vec3 &objectCentroid, const BBox &nodeBox, BBox &childBox, int &childIndex){
    const vec3 nodeCentroid = nodeBox.centroid();
    int index = 1;
    for(uint8_t dim = 0; dim<3; dim++){
        if(objectCentroid.e[dim]>nodeCentroid.e[dim]){
            childIndex+=index;
            //assign value to the child bounding box.
            childBox.bounds[0].e[dim] = nodeCentroid.e[dim];
            childBox.bounds[1].e[dim] = nodeBox.bounds[1].e[dim];
         }else{
            childBox.bounds[0].e[dim] = nodeBox.bounds[0].e[dim];
            childBox.bounds[1].e[dim] = nodeCentroid.e[dim];
        }
        index*=2;
    }
}


void Octree::insert(OctreeNode*& node, const Extent* extent,BBox &nodeBox, int depth){
    if(node->isLeaf){
        //currently the maximum depth is hardcode at 16
       if(node->nodeExtentsList.size()==0 || depth == 16) {
           node->nodeExtentsList.push_back(extent);
       }else{
           //Get those extents currently stored at the node and put them into the childrens
           node->isLeaf = false; // this will indicate the next recursion to assign the extent to its children
           while(node->nodeExtentsList.size()){
               insert(node, node->nodeExtentsList.back(), nodeBox,depth);
               node->nodeExtentsList.pop_back();
           }
           insert(node, extent, nodeBox, depth);
       }
    }
    else{
        //assign the input extent to one of the 8 children of the current node
        vec3 objectCentroid =  extent->centroid();    
        vec3 nodeCentroid = nodeBox.centroid();

        int childIndex = 0;
        BBox childBox;
        calculateChildBox(objectCentroid, nodeBox, childBox, childIndex);
        if(node->child[childIndex]==nullptr){
            node->child[childIndex] = new OctreeNode;
        }
        insert(node->child[childIndex], extent, childBox, depth+1);
    }
}
