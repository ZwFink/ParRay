#include "data_porting.h"
#include <vector>
#include <nlohmann/json.hpp>
#include "material.h"

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
    return 0;
}