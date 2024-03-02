#pragma once

#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include "Include.hpp"
#include "Renderer/Shader/GeometryShader/GeometryShader.hpp"
#include "Renderer/Shader/ScreenShader/ScreenShader.hpp"
#include "Renderer/Editor/ImGuiLayer.hpp"

class Shape;

class Renderer {
public:
    Renderer();
    ~Renderer();

    void Start(GLFWwindow* window);
    void Render();
    void Stop();

    Shape *shape;

// GBuffer
public:
    GLuint GetGBuffer() const { return _GBuffer;}
    GLuint GetAlbedoTexture() const { return _TAlbedo;}
    GLuint GetNormalTexture() const { return _TNormal;}
    GLuint GetPositionTexture() const { return _TPosition;}
    GLuint GetDepthTexture() const { return _TDepth;}
    GeometryShader &GetGeometryShader() { return _GeometryShader;}
private:
    GLuint _GBuffer;
    GLuint _TAlbedo, _TNormal, _TPosition, _TDepth;
    GeometryShader _GeometryShader;

// ScreenBuffer
public:
    GLuint GetScreenBuffer() const { return _ScreenBuffer;}
    GLuint GetScreenTexture() const { return _ScreenTexture;}
    ScreenShader &GetScreenShader() { return _ScreenShader;}
private:
    GLuint _ScreenBuffer;
    GLuint _ScreenTexture;
    ScreenShader _ScreenShader;
    GLuint _EmptyVAO;

// Editor
private:
    ImGuiLayer _ImGuiLayer;

// SINGLETON
    protected:
        static Renderer *instance;
    public:
        Renderer(Renderer &other) = delete;
        void operator=(const Renderer &) = delete;
        static Renderer *Get() {
            if (instance == nullptr)
                instance = new Renderer();
            return instance;
        };
};


#endif /* !RENDERER_HPP_ */