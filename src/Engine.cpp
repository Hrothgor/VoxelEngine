#include "Engine.hpp"
    
Engine *Engine::instance = nullptr;

Engine::Engine() {
}

Engine::~Engine() {
}

void Engine::Update(GLFWwindow *window) {
    float currentFrame = glfwGetTime();
    _deltaTime = currentFrame - _lastFrame;
    _lastFrame = currentFrame;
}

float Engine::GetDeltaTime() const {
    return _deltaTime;
}

int Engine::GetFPS() const {
    return (1 / _deltaTime);
}