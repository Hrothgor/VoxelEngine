#include "Camera.hpp"
#include "InputManager.hpp"
#include "Engine.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

Camera::Camera()
{
    UpdateViewMatrix();

    InputManager::Get()->Bind(GLFW_KEY_W, GLFW_PRESS, InputManager::InputType::KEY, [this](GLFWwindow *window) {
        _position += _moveSpeed * GetForward() * Engine::Get()->GetDeltaTime();
    });
    InputManager::Get()->Bind(GLFW_KEY_S, GLFW_PRESS, InputManager::InputType::KEY, [this](GLFWwindow *window) {
        _position -= _moveSpeed * GetForward() * Engine::Get()->GetDeltaTime();
    });
    InputManager::Get()->Bind(GLFW_KEY_A, GLFW_PRESS, InputManager::InputType::KEY, [this](GLFWwindow *window) {
        _position -= _moveSpeed * GetRight() * Engine::Get()->GetDeltaTime();
    });
    InputManager::Get()->Bind(GLFW_KEY_D, GLFW_PRESS, InputManager::InputType::KEY, [this](GLFWwindow *window) {
        _position += _moveSpeed * GetRight() * Engine::Get()->GetDeltaTime();
    });
    InputManager::Get()->Bind(GLFW_KEY_SPACE, GLFW_PRESS, InputManager::InputType::KEY, [this](GLFWwindow *window) {
        _position.y += _moveSpeed * Engine::Get()->GetDeltaTime();
    });
    InputManager::Get()->Bind(GLFW_KEY_LEFT_SHIFT, GLFW_PRESS, InputManager::InputType::KEY, [this](GLFWwindow *window) {
        _position.y -= _moveSpeed * Engine::Get()->GetDeltaTime();
    });

    InputManager::Get()->Bind(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, InputManager::InputType::MOUSE, [this](GLFWwindow *window) {
        if (_mouseClicked == true)
            return;

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        glfwGetCursorPos(window, &_mousePos.x, &_mousePos.y);

        _mouseClicked = true;
    });
    InputManager::Get()->Bind(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE, InputManager::InputType::MOUSE, [this](GLFWwindow *window) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
        glm::vec2 MouseOffset = {xpos - _mousePos.x, ypos - _mousePos.y};

        _rotation.y -= MouseOffset.x * _rotationSpeed * Engine::Get()->GetDeltaTime();
        _rotation.y = fmod(_rotation.y, 360.0f);

        _rotation.x -= MouseOffset.y * _rotationSpeed * Engine::Get()->GetDeltaTime();
        _rotation.x = glm::clamp(_rotation.x, -80.0f, 80.0f);


        //glfwSetCursorPos(window, _mousePos.x, _mousePos.y);
    }

    UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
    glm::mat4 matrix(1.0f);

    matrix = glm::rotate(matrix, glm::radians(_rotation.x), glm::vec3(1, 0, 0));
    matrix = glm::rotate(matrix, glm::radians(_rotation.y), glm::vec3(0, 1, 0));
    matrix = glm::rotate(matrix, glm::radians(_rotation.z), glm::vec3(0, 0, 1));

    matrix = glm::translate(matrix, -_position);

    _viewMatrix = glm::inverse(matrix);
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

glm::mat4 Camera::GetViewMatrix() const
{
    return (_viewMatrix);
}

glm::vec3 Camera::GetForward() const
{
    // Get forward from view matrix
    return (glm::vec3(_viewMatrix[2][0], _viewMatrix[2][1], _viewMatrix[2][2]));
}

glm::vec3 Camera::GetRight() const
{
    // Get right from view matrix
    return (glm::vec3(_viewMatrix[0][0], _viewMatrix[0][1], _viewMatrix[0][2]));
}

// VIEW MATRIX VIZUALIZATION
// ///////////////////////////////////////////////
// |    RIGHT.x     RIGHT.y     RIGHT.z     0    |
// |    UP.x        UP.y        UP.z        0    |
// |    FORWARD.x   FORWARD.y   FORWARD.z   0    |
// |    POS.x       POS.y       POS.z       1    |
// ///////////////////////////////////////////////