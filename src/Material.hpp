#pragma once

#ifndef MATERIAL_HPP_
#define MATERIAL_HPP_

#include "Include.hpp"

struct Material {
    glm::vec4 color = glm::vec4(0.0, 0.0, 0.0, 0.0);
    float reflectivity = 0.0;
    float roughness = 0.0;
    float metallic = 0.0;
    float emmisive = 0.0;
};


#endif /* !MATERIAL_HPP_ */