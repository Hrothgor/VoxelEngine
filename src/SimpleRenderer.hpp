#pragma once

#ifndef SIMPLERENDERER_HPP_
#define SIMPLERENDERER_HPP_

#include "Include.hpp"
#include "Shader/StaticShader/StaticShader.hpp"
#include "Camera.hpp"
#include "ImGuiLayer.hpp"

class SimpleRenderer {
public:
    SimpleRenderer();
    ~SimpleRenderer();

    void Init(GLFWwindow* window);

    void StartFrame();
    void EndFrame();
    void DrawFullScreenTriangle(const Camera &camera);

    GLuint _FrameBuffer;
    GLuint _Texture;
    StaticShader _Shader;
private:
    GLuint _EmptyVAO;
    GLuint _SSBO;

    ImGuiLayer _ImGuiLayer;

// SINGLETON
    protected:
        static SimpleRenderer *instance;
    public:
        SimpleRenderer(SimpleRenderer &other) = delete;
        void operator=(const SimpleRenderer &) = delete;
        static SimpleRenderer *Get() {
            if (instance == nullptr)
                instance = new SimpleRenderer();
            return instance;
        };
};


#endif /* !SIMPLERENDERER_HPP_ */