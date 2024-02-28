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
    // Smooth fps
    _fps += (1.0f / std::max<float>(_deltaTime, 0.00001) - _fps) * 0.03f;
}

float Engine::GetDeltaTime() const {
    return _deltaTime;
}

int Engine::GetFPS() const {
    return (_fps);
}