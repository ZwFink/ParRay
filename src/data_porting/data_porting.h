#ifndef _DATA_PORTING_H_
#define _DATA_PORTING_H_

#include <string>
#include <vector>
#include "sphere.h"
#include <nlohmann/json.hpp>


class ShapeDataIO
{
public:
    nlohmann::json serialize(const std::vector<std::shared_ptr<sphere>> &spheres);
    nlohmann::json serialize(std::shared_ptr<sphere> &sphere);
    nlohmann::json serialize(const std::shared_ptr<material> p_material);
    nlohmann::json serialize(const color &c);
    nlohmann::json serialize_location(const vec3 &location);
    void write(std::string fileName, nlohmann::json &j);
    std::vector<shared_ptr<sphere>> deserialize(nlohmann::json &json);
};

#endif