#include "data_porting.h"
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include "material.h"
#include "color.h"
#include <fstream>
#include <iomanip>

using json = nlohmann::json;

json ShapeDataIO::serialize(const std::shared_ptr<material> p_material){    
    json output;
    const material *pMaterial = p_material.get();
    if(auto p = dynamic_cast<const metal*>(pMaterial)){
        output["type"]="metal";
        output["color"]=serialize(p->albedo);
        output["fuzz"]=p->fuzz;
    }else if(auto p = dynamic_cast<const lambertian*>(pMaterial)){
        output["type"]="lambertian";
        output["color"]=serialize(p->albedo);
    }else if(auto p = dynamic_cast<const dielectric*>(pMaterial)){
        output["type"]="dielectric";
        output["refraction_index"]=p->ir;
    }
    return output;
}

json ShapeDataIO::serialize_location(vec3 const &location){
    json output;
    output["x"]=location.x();
    output["y"]=location.y();
    output["z"]=location.z();
    return output;
}

json ShapeDataIO::serialize(color const &c){
    json output;
    output["r"]=c.x();
    output["g"]=c.y();
    output["b"]=c.z();
    return output;
}

json ShapeDataIO::serialize(std::shared_ptr<sphere> &sphere){
    json output;
    output["location"]=serialize_location(sphere->center);
    output["material"]=serialize(sphere->mat_ptr);
    output["radius"]=sphere->radius;
    return output;
}

json ShapeDataIO::serialize(const std::vector<std::shared_ptr<sphere>> &spheres){
    json output;
    json sphereArray = json::array();
    for(auto  e: spheres){
        sphereArray.push_back(this->serialize(e));
    }
    output["spheres"]=sphereArray;
    return output;
}

void ShapeDataIO::write(std::string fileName, json& j){
    std::ofstream file;
    file.open(fileName);
    file<<std::setw(4)<<j<<std::endl;
    file.close();
}


vec3 deserialize_location(json &j){
    return vec3(j[''])
}

std::vector<shared_ptr<sphere>> ShapeDataIO::deserialize(json &j){
    std::vector<shared_ptr<sphere>> container;
    for(auto &e: j["spheres"]){

    }
    

}


