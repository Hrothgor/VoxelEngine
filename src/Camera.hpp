#pragma once

#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include "Include.hpp"

class Camera {
    public:
        Camera();
        ~Camera();

        void Update(GLFWwindow *window);
        void UpdateViewMatrix();

        glm::vec3 GetPosition() const;
        glm::vec3 GetRotation() const;
        float GetPitch() const;
        float GetYaw() const;
        float GetRoll() const;

        glm::mat4 GetViewMatrix() const;

        glm::vec3 GetForward() const;
        glm::vec3 GetRight() const;

    protected:
    private:
        glm::vec3 _position = {-10.0f, 60.0f ,-10.0f};
        glm::vec3 _rotation = {-10.0f, -30.0f, .0f}; //pitch yaw roll
        glm::mat4 _viewMatrix;

        float _moveSpeed = 30.0;
        float _rotationSpeed = 20.0;

        bool _mouseClicked;
        glm::vec<2, double> _mousePos;
};

#endif /* !CAMERA_HPP_ */