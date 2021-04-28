#include <gtest/gtest.h>
#include "data_porting.h"
#include <nlohmann/json.hpp>
#include "material.h"

TEST(serializing, color){
    ShapeDataIO io;
    color c(0.1,0.2,0.3);
    nlohmann::json output = io.serialize(c);
    std::string actual = output.dump();
    std::string expected = "{\"b\":0.3,\"g\":0.2,\"r\":0.1}";
    ASSERT_EQ(expected, actual);
}

TEST(serializing, metal){
    ShapeDataIO io;
    std::shared_ptr<metal> m1 = make_shared<metal>(vec3(4,5,6),7.9);
    nlohmann::json output = io.serialize(m1);
    std::string actual = output.dump();
    std::string expected = "{\"color\":{\"b\":6.0,\"g\":5.0,\"r\":4.0},\"fuzz\":1.0,\"type\":\"metal\"}";
    ASSERT_EQ(expected, actual);
    std::cout<<actual<<std::endl;
}

TEST(serializing, lambertian){
    ShapeDataIO io;
    auto m1 = make_shared<lambertian>(vec3(4,5,6));
    nlohmann::json output = io.serialize(m1);
    std::string actual = output.dump();
    std::string expected = "{\"color\":{\"b\":6.0,\"g\":5.0,\"r\":4.0},\"type\":\"lambertian\"}";
    ASSERT_EQ(expected, actual);
}

TEST(serializing, dielectric){
    ShapeDataIO io;
    auto m1 = make_shared<dielectric>(3.14);
    nlohmann::json output = io.serialize(m1);
    std::string actual = output.dump();
    std::string expected = "{\"refraction_index\":3.14,\"type\":\"dielectric\"}";
    ASSERT_EQ(expected, actual);
}

TEST(serializing, sphere){
    ShapeDataIO io;
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto s1 = make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material);
    nlohmann::json output = io.serialize(s1);
    std::string actual = output.dump();
    std::string expected = "{\"sphere\":{\"location\":{\"x\":0.0,\"y\":-1000.0,\"z\":0.0},\"material\":{\"color\":{\"b\":0.5,\"g\":0.5,\"r\":0.5},\"type\":\"lambertian\"},\"radius\":1000.0}}";
    ASSERT_EQ(expected, actual);
}

TEST(deserializing, sphere){
    ShapeDataIO io;
    std::string input = "{\"spheres\":[{\"sphere\":{\"location\":{\"x\":0.0,\"y\":-1000.0,\"z\":0.0},\"material\":{\"color\":{\"b\":0.5,\"g\":0.6,\"r\":0.7},\"type\":\"lambertian\"},\"radius\":1000.0}}]}";
    nlohmann::json j = nlohmann::json::parse(input);
    std::vector<shared_ptr<sphere>> parsed = io.deserialize_spheres(j);
    ASSERT_EQ(1,parsed.size());
    ASSERT_DOUBLE_EQ(0, parsed[0]->center.x());
    ASSERT_DOUBLE_EQ(-1000, parsed[0]->center.y());
    ASSERT_DOUBLE_EQ(0, parsed[0]->center.z());
    ASSERT_TRUE(std::dynamic_pointer_cast<lambertian>(parsed[0]->mat_ptr)!=nullptr);
    ASSERT_DOUBLE_EQ(0.7, std::dynamic_pointer_cast<lambertian>(parsed[0]->mat_ptr)->albedo.x());
    ASSERT_DOUBLE_EQ(0.6, std::dynamic_pointer_cast<lambertian>(parsed[0]->mat_ptr)->albedo.y());
    ASSERT_DOUBLE_EQ(0.5, std::dynamic_pointer_cast<lambertian>(parsed[0]->mat_ptr)->albedo.z());
    ASSERT_DOUBLE_EQ(1000, parsed[0]->radius);
}


TEST(deserializing, Sphere){
     ShapeDataIO io;
    std::string input = "{\"spheres\":[{\"sphere\":{\"location\":{\"x\":0.0,\"y\":-1000.0,\"z\":0.0},\"material\":{\"color\":{\"b\":0.5,\"g\":0.6,\"r\":0.7},\"type\":\"lambertian\"},\"radius\":1000.0}}]}";
    nlohmann::json j = nlohmann::json::parse(input);
    std::vector<shared_ptr<Sphere>> parsed = io.deserialize_Spheres(j);
    ASSERT_EQ(1,parsed.size());
    ASSERT_DOUBLE_EQ(0, parsed[0]->center.x());
    ASSERT_DOUBLE_EQ(-1000, parsed[0]->center.y());
    ASSERT_DOUBLE_EQ(0, parsed[0]->center.z());
    ASSERT_TRUE(std::dynamic_pointer_cast<lambertian>(parsed[0]->mat_ptr)!=nullptr);
    ASSERT_DOUBLE_EQ(0.7, std::dynamic_pointer_cast<lambertian>(parsed[0]->mat_ptr)->albedo.x());
    ASSERT_DOUBLE_EQ(0.6, std::dynamic_pointer_cast<lambertian>(parsed[0]->mat_ptr)->albedo.y());
    ASSERT_DOUBLE_EQ(0.5, std::dynamic_pointer_cast<lambertian>(parsed[0]->mat_ptr)->albedo.z());
    ASSERT_DOUBLE_EQ(1000, parsed[0]->r);
}