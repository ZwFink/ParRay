#ifndef _DATA_PORTING_H_
#define _DATA_PORTING_H_

#include <string>
#include <vector>
#include "sphere.h"
#include <nlohmann/json.hpp>
#include "boundable.h"


class ShapeDataIO
{
public:
    nlohmann::json serialize(const std::vector<std::shared_ptr<sphere>> &spheres);
    nlohmann::json serialize(std::shared_ptr<sphere> &sphere);
    nlohmann::json serialize(const std::shared_ptr<material> p_material);
    nlohmann::json serialize(const color &c);
    nlohmann::json serialize_location(const vec3 &location);

    void write(std::string fileName, nlohmann::json &j);

    nlohmann::json read(std::string fileName);

    std::vector<shared_ptr<sphere>> deserialize_spheres(const nlohmann::json &j);
    std::vector<shared_ptr<Sphere>> deserialize_Spheres(const nlohmann::json &j);
};

#endif