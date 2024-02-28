#pragma once

#ifndef TRANSFORM_HPP_
#define TRANSFORM_HPP_

#include "Include.hpp"

class Transform {
public:
    Transform(const glm::vec3 &position = glm::vec3(0.0f), const glm::vec3 &rotation = glm::vec3(0.0f), const glm::vec3 &scale = glm::vec3(1.0f));
    ~Transform();

    void SetPosition(const glm::vec3 &position) { _Position = position; }
    void SetRotation(const glm::vec3 &rotation) { _Rotation = rotation; }
    void SetScale(const glm::vec3 &scale) { _Scale = scale; }

    void Move(const glm::vec3 &position) { _Position += position; }
    void Rotate(const glm::vec3 &rotation) { _Rotation += rotation; }
    void Scale(const glm::vec3 &scale) { _Scale += scale; }

    const glm::vec3 &GetPosition() const { return _Position; }
    const glm::vec3 &GetRotation() const { return _Rotation; }
    const glm::vec3 &GetScale() const { return _Scale; }

    glm::mat4 GetTransformMatrix() const;
protected:
private:
    glm::vec3 _Position;
    glm::vec3 _Rotation;
    glm::vec3 _Scale;
};


#endif /* !TRANSFORM_HPP_ */