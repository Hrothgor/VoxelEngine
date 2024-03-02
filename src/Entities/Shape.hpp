#pragma once

#ifndef SHAPE_HPP_
#define SHAPE_HPP_

#include "Include.hpp"
#include "Transform.hpp"

#define MAX_MATERIALS 255

struct Material {
    glm::vec4 color = glm::vec4(0.0, 0.0, 0.0, 0.0);
    float reflectivity = 0.0;
    float roughness = 0.0;
    float metallic = 0.0;
    float emmisive = 0.0;
};

class Shape {
public:
    Shape();
    ~Shape();

    void Build(); // TODO: Remove this function
    void GenTexture();

// DATA
public:
    glm::u16vec3 GetSize() const { return _Size; }
    void SetSize(const glm::u16vec3 &size) { 
        _Size = size;
        _Size.x = std::clamp<int>(_Size.x, 1, 256);
        _Size.y = std::clamp<int>(_Size.y, 1, 256);
        _Size.z = std::clamp<int>(_Size.z, 1, 256);
    }
    const std::vector<int8_t> &GetData() const { return _Data; }
    void SetData(const std::vector<int8_t> &data) { _Data = data; }

    void SetMaterial(int index, const Material &material);

    Transform &GetTransform() { return _Transform; }
private:
    glm::u16vec3 _Size;
    std::vector<int8_t> _Data; // 0 = empty, 1 = full
    Material _MaterialPalette[MAX_MATERIALS];
    Transform _Transform;

// OPENGL
public:
    GLuint GetVolumeTexture() const { return _VolumeTexture; }
    GLuint GetMaterialTexture() const { return _MaterialTexture; }
private:
    GLuint _VolumeTexture = 0;
    GLuint _MaterialTexture = 0;
};


#endif /* !SHAPE_HPP_ */