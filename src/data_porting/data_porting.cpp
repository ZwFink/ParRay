#include "data_porting.h"
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include "material.h"
#include "color.h"
#include <fstream>
#include <iomanip>
#include "boundable.h"

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
    output["sphere"]["location"]=serialize_location(sphere->center);
    output["sphere"]["material"]=serialize(sphere->mat_ptr);
    output["sphere"]["radius"]=sphere->radius;
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

vec3 deserialize_location(const json &j){
    return vec3(j["x"].get<double>(),
    j["y"].get<double>(),
    j["z"].get<double>());
}

color deserialize_color(const json &j){
    return color(j["r"].get<double>(),j["g"].get<double>(),j["b"].get<double>());
}

std::shared_ptr<metal> deserialize_metal(const json &j){
    color c = deserialize_color(j["color"]);
    double fuzz = j["fuzz"].get<double>();
    return make_shared<metal>(c, fuzz);
}

std::shared_ptr<dielectric> deserialize_dielectric(const json &j){
    double refraction_index = j["refraction_index"].get<double>();
    return make_shared<dielectric>(refraction_index);
}

std::shared_ptr<lambertian> deserialize_lambertian(const json &j){
    color c = deserialize_color(j["color"]);
    return make_shared<lambertian>(c);
}

std::shared_ptr<material> deserialize_material(const json &j){
   std::string type = j["type"].get<std::string>();
   if(type.compare("lambertian")==0){
       return std::dynamic_pointer_cast<material>(deserialize_lambertian(j));
   }else if(type.compare("dielectric")==0){
       return std::dynamic_pointer_cast<dielectric>(deserialize_dielectric(j));
   }else if(type.compare("metal")==0){
       return std::dynamic_pointer_cast<metal>(deserialize_metal(j));
   }else{
       std::cerr<<"Unknown material type"<<std::endl;
   }
}

std::shared_ptr<sphere> deserialize_sphere(const json &j){
    vec3 location = deserialize_location(j["location"]);
    std::shared_ptr<material> m = deserialize_material(j["material"]);
    double radius = j["radius"].get<double>();
    return make_shared<sphere>(location, radius, m);
}

std::vector<shared_ptr<sphere>> ShapeDataIO::deserialize_spheres(const json &j){
    std::vector<shared_ptr<sphere>> container;
    for(const auto &e:j["spheres"]){
        container.push_back(deserialize_sphere(e["sphere"]));
    }
    return container;
}

std::shared_ptr<Sphere> deserialize_Sphere(const json &j){
    vec3 location = deserialize_location(j["location"]);
    std::shared_ptr<material> m = deserialize_material(j["material"]);
    double radius = j["radius"].get<double>();
    return make_shared<Sphere>(location, radius, m);
}

std::vector<shared_ptr<Sphere>> ShapeDataIO::deserialize_Spheres(const nlohmann::json &j){
    std::vector<shared_ptr<Sphere>> container;
    for(const auto &e:j["spheres"]){
        container.push_back(deserialize_Sphere(e["sphere"]));
    }
    return container;
}

json ShapeDataIO::read(std::string fileName){
    json j;
    std::ifstream file(fileName);
    if(!file.good()){
        std::cerr<<"File "<<fileName<<" not exist"<<std::endl;
        exit(1);
    }
    file.open(fileName);
    //std::cout<<file.rdbuf();
    file>>j;
    file.close();
    return j;
}
