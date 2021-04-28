#include "data_porting.h"
#include <vector>
#include <nlohmann/json.hpp>
#include "material.h"


bool fequals(double a, double b){
    return fabs(a-b)<0.001;
}

int main(int argc, char **argv)
{
    ShapeDataIO io;
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto glass_material = make_shared<dielectric>(1.5);

    //metal
    auto albedo = color::random(0.5, 1);
    auto fuzz = random_double(0, 0.5);
    auto metal_material = make_shared<metal>(albedo, fuzz);

    std::vector<shared_ptr<sphere>> spheres;
    spheres.push_back(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));
    spheres.push_back(make_shared<sphere>(point3(5,1.5,1),1,glass_material));
    spheres.push_back(make_shared<sphere>(point3(1,1,1),1,metal_material));

    auto json = io.serialize(spheres);
    io.write("sphere.data",json);
    const nlohmann::json readJson = io.read("sphere.data");
    auto readSpheres = io.deserialize_Spheres(readJson);

    for(int i=0; i<3;i++){
        if(
           !(fequals(readSpheres[i]->center.x(),spheres[i]->center.x()) &&
           fequals(readSpheres[i]->center.y(),spheres[i]->center.y()) &&
           fequals(readSpheres[i]->center.z(),spheres[i]->center.z()) &&
           fequals(readSpheres[i]->r,spheres[i]->radius))){
               std::cout<<"Failed test"<<std::endl;
           }
    }
    
    return 0;
}