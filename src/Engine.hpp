#pragma once

#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include "Include.hpp"

class Camera;
class Shape;

class Engine {
    public:
        Engine();
        ~Engine();

        void Start();
        void Update();
        void Stop();

// WINDOW
public:
    GLFWwindow *GetWindow() { return _Window; }
    int GetWidth() const { return _Width; }
    int GetHeight() const { return _Height; }
    void SetWidth(int width) { _Width = width; }
    void SetHeight(int height) { _Height = height; }
    std::string GetTitle() const { return _Title; }
    bool GetVSync() const { return _VSync; }
    void SetVSync(bool vsync) { _VSync = vsync; glfwSwapInterval(_VSync); }
private:
    GLFWwindow *_Window;
    int _Width, _Height;
    std::string _Title;
    bool _VSync;

// SHAPES
    std::vector<std::shared_ptr<Shape>> _shapes;

// MainCamera
    public:
        std::shared_ptr<Camera> GetMainCamera() { return _MainCamera; }
    private:
        std::shared_ptr<Camera> _MainCamera;

// TIME
    public:
        float GetDeltaTime();
        int GetFPS();
    private:
        void UpdateAverageFPS();
        std::vector<float> _fpsTimes;
        float _averageFPS;

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