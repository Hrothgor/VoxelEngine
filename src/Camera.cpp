#include "Camera.hpp"
#include "InputManager.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

Camera::Camera()
{
    InputManager::Get()->Bind(GLFW_KEY_W, GLFW_PRESS, InputManager::InputType::KEY, [this](GLFWwindow *window) {
        _position.z += _moveSpeed;
    });
    InputManager::Get()->Bind(GLFW_KEY_S, GLFW_PRESS, InputManager::InputType::KEY, [this](GLFWwindow *window) {
        _position.z -= _moveSpeed;
    });
    InputManager::Get()->Bind(GLFW_KEY_A, GLFW_PRESS, InputManager::InputType::KEY, [this](GLFWwindow *window) {
        _position.x -= _moveSpeed;
    });
    InputManager::Get()->Bind(GLFW_KEY_D, GLFW_PRESS, InputManager::InputType::KEY, [this](GLFWwindow *window) {
        _position.x += _moveSpeed;
    });
    InputManager::Get()->Bind(GLFW_KEY_SPACE, GLFW_PRESS, InputManager::InputType::KEY, [this](GLFWwindow *window) {
        _position.y += _moveSpeed;
    });
    InputManager::Get()->Bind(GLFW_KEY_LEFT_SHIFT, GLFW_PRESS, InputManager::InputType::KEY, [this](GLFWwindow *window) {
        _position.y -= _moveSpeed;
    });

    InputManager::Get()->Bind(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, InputManager::InputType::MOUSE, [this](GLFWwindow *window) {
        if (_mouseClicked == true)
            return;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        _mousePos = glm::vec2(xpos, ypos);
        _mouseClicked = true;
    });
    InputManager::Get()->Bind(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE, InputManager::InputType::MOUSE, [this](GLFWwindow *window) {
        _mouseClicked = false;
    });
}

Camera::~Camera()
{
}

void Camera::Update(GLFWwindow *window)
{
    if (_mouseClicked == true) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        _rotation.y += (xpos - _mousePos.x) * _rotationSpeed;
        if (_rotation.y > 360.0f)
            _rotation.y -= 360.0f;
        else if (_rotation.y < -360.0f)
            _rotation.y += 360.0f;

        _rotation.x += (ypos - _mousePos.y) * _rotationSpeed;
        _rotation.x = glm::clamp(_rotation.x, -80.0f, 80.0f);

    
        _mousePos = glm::vec2(xpos, ypos);
    }
    std::cout << "Camera rotation: " << _rotation.x << " " << _rotation.y << " " << _rotation.z << std::endl;
}

glm::vec3 Camera::GetPosition() const
{
    return (_position);
}

glm::vec3 Camera::GetRotation() const
{
    return (_rotation);
}

float Camera::GetPitch() const
{
    return (_rotation.x);
}

float Camera::GetYaw() const
{
    return (_rotation.y);
}

float Camera::GetRoll() const
{
    return (_rotation.z);
}

glm::vec3 Camera::GetForward() const
{
    glm::vec3 forward;

    forward.x = cos(glm::radians(_rotation.y)) * cos(glm::radians(_rotation.x));
    forward.y = sin(glm::radians(_rotation.x));
    forward.z = sin(glm::radians(_rotation.y)) * cos(glm::radians(_rotation.x));
    return (glm::normalize(forward));
}

glm::vec3 Camera::GetRight() const
{
    glm::vec3 right;

    right.x = sin(glm::radians(_rotation.y) - M_PI / 2);
    right.y = 0;
    right.z = cos(glm::radians(_rotation.y) - M_PI / 2);
    return (glm::normalize(right));
}