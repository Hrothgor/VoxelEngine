#pragma once

#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include "Include.hpp"

class Engine {
    public:
        Engine();
        ~Engine();

        void Update(GLFWwindow *window);

        float GetDeltaTime() const;
        int GetFPS() const;

    protected:
    private:
        float _deltaTime;
        float _lastFrame;
        float _fps = 60.0f;

// SINGLETON
    protected:
        static Engine *instance;
    public:
        Engine(Engine &other) = delete;
        void operator=(const Engine &) = delete;
        static Engine *Get() {
            if (instance == nullptr)
                instance = new Engine();
            return instance;
        };
};

#endif /* !ENGINE_HPP_ */