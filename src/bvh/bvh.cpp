#include "bvh.hpp"
#include <stdio.h>

int main(int argc, char** argv){
    printf("Hello from bvh\n");
    return 0;
}

Extent::Extent(){
        for (uint8_t i = 0; i < kNumPlaneSetNormals; ++i)
        {
            d[i][0] = DBL_MAX;
            d[i][1] = -DBL_MAX;
        } 
}

void Extent::extendBy(const Extent &extents){
        for (int i = 0; i < kNumPlaneSetNormals; ++i)
        {
            if (extents.d[i][0] < d[i][0])
                d[i][0] = extents.d[i][0];
            if (extents.d[i][1] > d[i][1])
                d[i][1] = extents.d[i][1];
        } 
}

bool Extent::interset(const double *numberator, const double *denominator, double &tNear, double &tFar, int &planeIndex){
    for(int i=0; i<kNumPlaneSetNormals;i++){
            double tn = (d[i][0]-numberator[i])/denominator[i];
            double tf = (d[i][1]-numberator[i])/denominator[i];
            if(denominator[i]<0) std::swap(tn, tf);
            if(tn>tNear) tNear = tn, planeIndex =i;
            if(tf<tFar) tFar = tf;
            if(tNear >tFar) return false;
        }
        return true;    
}

