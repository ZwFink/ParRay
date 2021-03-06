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

void ShapeDataIO::clear_scene(std::vector<Sphere*> &input){
  for(int i=0; i<input.size();i++){
    delete input[i];
    input[i]=nullptr;
  }
}

std::vector<Sphere*> ShapeDataIO::load_scene(std::string fileName){
  nlohmann::json j = this->read(fileName);
  return this->deserialize_Spheres(j);
}

json ShapeDataIO::serialize(const material *pMaterial){    
    json output;
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

json ShapeDataIO::serialize(sphere *sphere){
    json output;
    output["sphere"]["location"]=serialize_location(sphere->center);
    output["sphere"]["material"]=serialize(sphere->mat_ptr.get());
    output["sphere"]["radius"]=sphere->radius;
    return output;
}

json ShapeDataIO::serialize(const std::vector<sphere*> &spheres){
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

metal* deserialize_metal(const json &j){
    color c = deserialize_color(j["color"]);
    double fuzz = j["fuzz"].get<double>();
    return new metal(c, fuzz);
}

dielectric* deserialize_dielectric(const json &j){
    double refraction_index = j["refraction_index"].get<double>();
    return new dielectric(refraction_index);
}

lambertian* deserialize_lambertian(const json &j){
    color c = deserialize_color(j["color"]);
    return new lambertian(c);
}

material* deserialize_material(const json &j){
   std::string type = j["type"].get<std::string>();
   if(type.compare("lambertian")==0){
       return dynamic_cast<material*>(deserialize_lambertian(j));
   }else if(type.compare("dielectric")==0){
       return dynamic_cast<material*>(deserialize_dielectric(j));
   }else if(type.compare("metal")==0){
       return dynamic_cast<material*>(deserialize_metal(j));
   }else{
       std::cerr<<"Unknown material type"<<std::endl;
   }
}

sphere* deserialize_sphere(const json &j){
    vec3 location = deserialize_location(j["location"]);
    material* m = deserialize_material(j["material"]);
    double radius = j["radius"].get<double>();
    return new sphere(location, radius, unique_ptr<material>(m));
}

std::vector<sphere*> ShapeDataIO::deserialize_spheres(const json &j){
    std::vector<sphere*> container;
    for(const auto &e:j["spheres"]){
        container.push_back(deserialize_sphere(e["sphere"]));
    }
    return container;
}

Sphere* deserialize_Sphere(const json &j){
    vec3 location = deserialize_location(j["location"]);
    material* m = deserialize_material(j["material"]);
    double radius = j["radius"].get<double>();
    return new Sphere(location, radius, m);
}

std::vector<Sphere*> ShapeDataIO::deserialize_Spheres(const nlohmann::json &j){
    std::vector<Sphere*> container;
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
