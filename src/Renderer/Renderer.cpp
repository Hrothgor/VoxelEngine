#include "Renderer/Renderer.hpp"
#include "Engine.hpp"  
#include "Logger.hpp"
#include "Profiler.hpp"
#include "Entities/Shape.hpp"
#include "Entities/Camera.hpp"

#include "Loader.hpp"

Renderer *Renderer::instance = nullptr;

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::Start(GLFWwindow* window)
{
    glGenVertexArrays(1, &_EmptyVAO);

    shape = Loader::ConvertVoxToShape(Loader::LoadFromVoxFile("res/vox/monu3.vox"));
    shape->GenTexture();

    // Create gBuffer and texture to render to it
    glGenFramebuffers(1, &_GBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _GBuffer);
    // - albedo color buffer
    glGenTextures(1, &_TAlbedo);
    glBindTexture(GL_TEXTURE_2D, _TAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _TAlbedo, 0);
    // - normal color buffer
    glGenTextures(1, &_TNormal);
    glBindTexture(GL_TEXTURE_2D, _TNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _TNormal, 0);
    // - position color buffer
    glGenTextures(1, &_TPosition);
    glBindTexture(GL_TEXTURE_2D, _TPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _TPosition, 0);
    // - depth color buffer
    glGenTextures(1, &_TDepth);
    glBindTexture(GL_TEXTURE_2D, _TDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _TDepth, 0);

    unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Logger::Get()->Log(Logger::LogType::ERROR, "ERROR::FRAMEBUFFER:: GFramebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Create ScreenBuffer and texture to render to it
    glGenFramebuffers(1, &_ScreenBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _ScreenBuffer);
    // - Screen color buffer
    glGenTextures(1, &_ScreenTexture);
    glBindTexture(GL_TEXTURE_2D, _ScreenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ScreenTexture, 0);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Logger::Get()->Log(Logger::LogType::ERROR, "ERROR::FRAMEBUFFER:: ScreenFramebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    _ImGuiLayer.Start(window);
}

void Renderer::Render()
{
    Profiler::Get()->StartFrame("GPU Frame");

    glClearColor(0.0f, 0.0f, 0.0f, 1.00f);
    glViewport(0, 0, 1920, 1080);

    _ImGuiLayer.BeginFrame();
    _ImGuiLayer.ImGuiRender();
    _ImGuiLayer.EndFrame();

    {
        Profiler::Get()->StartFrame("GPU::GBuffer");
        glBindFramebuffer(GL_FRAMEBUFFER, _GBuffer);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, shape->GetVolumeTexture());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shape->GetMaterialTexture());

        _GeometryShader.Begin();
        _GeometryShader.LoadTime(glfwGetTime());
        GLint ViewportSize[4];
        glGetIntegerv(GL_VIEWPORT, ViewportSize);
        _GeometryShader.LoadResolution(glm::vec2(ViewportSize[2], ViewportSize[3]));
        _GeometryShader.LoadCameraViewMatrix(Engine::Get()->GetMainCamera()->GetViewMatrix());
        _GeometryShader.LoadTransformMatrix(shape->GetTransform().GetTransformMatrix());
        _GeometryShader.LoadTextureVolume(0);
        _GeometryShader.LoadTextureMaterial(1);

        glBindVertexArray(_EmptyVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        _GeometryShader.End();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_3D, 0);
        glFinish(); // TODO: TAKE THIS OFF, THIS IS FOR ACCURATE GPU PROFILING
        Profiler::Get()->EndFrame();
    }

    {
        Profiler::Get()->StartFrame("GPU::ScreenBuffer");
        glBindFramebuffer(GL_FRAMEBUFFER, _ScreenBuffer);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _TAlbedo);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _TNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, _TPosition);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, _TDepth);

        _ScreenShader.Begin();
        GLint ViewportSize[4];
        glGetIntegerv(GL_VIEWPORT, ViewportSize);
        _ScreenShader.LoadResolution(glm::vec2(ViewportSize[2], ViewportSize[3]));
        _ScreenShader.LoadCameraViewMatrix(Engine::Get()->GetMainCamera()->GetViewMatrix());
        _ScreenShader.LoadTextureAlbedo(0);
        _ScreenShader.LoadTextureNormal(1);
        _ScreenShader.LoadTexturePosition(2);
        _ScreenShader.LoadTextureDepth(3);

        glBindVertexArray(_EmptyVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        _ScreenShader.End();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFinish(); // TODO: TAKE THIS OFF, THIS IS FOR ACCURATE GPU PROFILING
        Profiler::Get()->EndFrame();
    }

    Profiler::Get()->EndFrame();
}

void Renderer::Stop()
{
    _GeometryShader.Stop();
    _ScreenShader.Stop();
    _ImGuiLayer.Stop();
}