#pragma once

#ifndef SIMPLERENDERER_HPP_
#define SIMPLERENDERER_HPP_

#include "Include.hpp"
#include "Shader/StaticShader/StaticShader.hpp"
#include "Camera.hpp"
#include "ImGuiLayer.hpp"

class SimpleRenderer {
public:
    SimpleRenderer(GLFWwindow* window);
    ~SimpleRenderer();

    void Init();

    void StartFrame();
    void EndFrame();
    void DrawFullScreenTriangle(const Camera &camera);

private:
    StaticShader _Shader;
    GLuint _EmptyVAO;
    GLuint _SSBO;

    ImGuiLayer _ImGuiLayer;
};


#endif /* !SIMPLERENDERER_HPP_ */