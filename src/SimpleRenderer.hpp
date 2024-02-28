#pragma once

#ifndef SIMPLERENDERER_HPP_
#define SIMPLERENDERER_HPP_

#include "Include.hpp"
#include "Shader/GeometryShader/GeometryShader.hpp"
#include "Shader/ScreenShader/ScreenShader.hpp"
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

    GLuint _gBuffer;
    GLuint _gAlbedo, _gNormal, _gPosition, _gDepth;
    GLuint _finalBuffer;
    GLuint _finalTexture;
    GeometryShader _GeometryShader;
    ScreenShader _ScreenShader;
private:
    GLuint _EmptyVAO;

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