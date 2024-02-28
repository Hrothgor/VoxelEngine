#include "SimpleRenderer.hpp"
#include "Engine.hpp"  
#include "Logger.hpp"
#include "Shape.hpp"

SimpleRenderer *SimpleRenderer::instance = nullptr;

SimpleRenderer::SimpleRenderer()
{
}

SimpleRenderer::~SimpleRenderer()
{
    _GeometryShader.Destroy();
    _ScreenShader.Destroy();
    _ImGuiLayer.End();
}

Shape shape(256);

void SimpleRenderer::Init(GLFWwindow* window)
{
    glGenVertexArrays(1, &_EmptyVAO);

    shape.Build();
    shape.GenTexture();

    // Create gBuffer and texture to render to it
    glGenFramebuffers(1, &_gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
    // - albedo color buffer
    glGenTextures(1, &_gAlbedo);
    glBindTexture(GL_TEXTURE_2D, _gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gAlbedo, 0);
    // - normal color buffer
    glGenTextures(1, &_gNormal);
    glBindTexture(GL_TEXTURE_2D, _gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gNormal, 0);
    // - position color buffer
    glGenTextures(1, &_gPosition);
    glBindTexture(GL_TEXTURE_2D, _gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gPosition, 0);
    // - depth color buffer
    glGenTextures(1, &_gDepth);
    glBindTexture(GL_TEXTURE_2D, _gDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _gDepth, 0);

    unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Logger::Get()->Log(Logger::LogType::ERROR, "ERROR::FRAMEBUFFER:: GFramebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Create finalBuffer and texture to render to it
    glGenFramebuffers(1, &_finalBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _finalBuffer);
    // - final color buffer
    glGenTextures(1, &_finalTexture);
    glBindTexture(GL_TEXTURE_2D, _finalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _finalTexture, 0);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Logger::Get()->Log(Logger::LogType::ERROR, "ERROR::FRAMEBUFFER:: FinalFramebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    _ImGuiLayer.Start(window);
}

void SimpleRenderer::StartFrame()
{
    glEnable(GL_MULTISAMPLE);
    glClearColor(1.00f, 0.49f, 0.04f, 1.00f);
    glViewport(0, 0, 1920, 1080);
}

void SimpleRenderer::EndFrame()
{
}

void SimpleRenderer::DrawFullScreenTriangle(const Camera &camera)
{
    _ImGuiLayer.BeginFrame();
    _ImGuiLayer.ImGuiRender();
    _ImGuiLayer.EndFrame();

    {
        glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, shape.GetVolumeTexture());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shape.GetMaterialTexture());

        _GeometryShader.Start();
        _GeometryShader.LoadTime(glfwGetTime());
        GLint ViewportSize[4];
        glGetIntegerv(GL_VIEWPORT, ViewportSize);
        _GeometryShader.LoadResolution(glm::vec2(ViewportSize[2], ViewportSize[3]));
        _GeometryShader.LoadCameraViewMatrix(camera.GetViewMatrix());
        _GeometryShader.LoadTransformMatrix(shape.GetTransform().GetTransformMatrix());
        _GeometryShader.LoadTextureVolume(0);
        _GeometryShader.LoadTextureMaterial(1);

        glBindVertexArray(_EmptyVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        _GeometryShader.Stop();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_3D, 0);
    }

    {
        glBindFramebuffer(GL_FRAMEBUFFER, _finalBuffer);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _gAlbedo);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, _gPosition);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, _gDepth);

        _ScreenShader.Start();
        GLint ViewportSize[4];
        glGetIntegerv(GL_VIEWPORT, ViewportSize);
        _ScreenShader.LoadResolution(glm::vec2(ViewportSize[2], ViewportSize[3]));
        _ScreenShader.LoadTextureAlbedo(0);
        _ScreenShader.LoadTextureNormal(1);
        _ScreenShader.LoadTexturePosition(2);
        _ScreenShader.LoadTextureDepth(3);

        glBindVertexArray(_EmptyVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        _ScreenShader.Stop();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}