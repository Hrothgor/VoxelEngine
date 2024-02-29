#pragma once

#ifndef SHAPE_HPP_
#define SHAPE_HPP_

#include "Include.hpp"
#include "Transform.hpp"
#include "Material.hpp"

#define MAX_MATERIALS 252

class Shape {
public:
    Shape(int size = 256);
    ~Shape();

    void Build();
    void GenTexture();
    
    int GetSize() const { return _Size; }
    const std::vector<int8_t>& GetData() const { return _Data; }

    Transform &GetTransform() { return _Transform; }
    void SetMaterial(int index, const Material &material);

    GLuint GetVolumeTexture() const { return _VolumeTexture; }
    GLuint GetMaterialTexture() const { return _MaterialTexture; }
protected:
private:
    int _Size;
    std::vector<int8_t> _Data; // 0 = empty, 1 = full

    Transform _Transform;
    Material _MaterialPalette[MAX_MATERIALS];

    GLuint _VolumeTexture = 0;
    GLuint _MaterialTexture = 0;
};


#endif /* !SHAPE_HPP_ */