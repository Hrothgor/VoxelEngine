#pragma once

#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include "Include.hpp"

class Camera {
    public:
        Camera();
        ~Camera();

        void Update(GLFWwindow *window);

        glm::vec3 GetPosition() const;
        glm::vec3 GetRotation() const;
        float GetPitch() const;
        float GetYaw() const;
        float GetRoll() const;

        glm::vec3 GetForward() const;
        glm::vec3 GetRight() const;

    protected:
    private:
        glm::vec3 _position = {.0f, .0f ,.0f};
        glm::vec3 _rotation = {.0f, .0f, .0f}; //pitch yaw roll

        float _moveSpeed = 0.2f;
        float _rotationSpeed = 0.1f;

        glm::vec2 _mousePos;
        bool _mouseClicked;
};

#endif /* !CAMERA_HPP_ */