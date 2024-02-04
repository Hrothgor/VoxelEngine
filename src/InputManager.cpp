#include "InputManager.hpp"
    
InputManager *InputManager::instance = nullptr;

InputManager::InputManager() {
}

InputManager::~InputManager() {
}

void InputManager::Bind(int key, int action, InputType type, std::function<void(GLFWwindow *)> callback) {
    _keysCallback[{type, key, action}] = callback;
}

void InputManager::ProcessInput(GLFWwindow *window) {
    for (auto &[keyAction, callback] : _keysCallback) {
        if ((keyAction.type == KEY && glfwGetKey(window, keyAction.key) == keyAction.action)
            || (keyAction.type == MOUSE && glfwGetMouseButton(window, keyAction.key) == keyAction.action)) {
            callback(window);
        }
    }
}