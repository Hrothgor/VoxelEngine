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
    _Shader.Destroy();
    _ImGuiLayer.End();
}

void SimpleRenderer::Init(GLFWwindow* window)
{
    glGenVertexArrays(1, &_EmptyVAO);

    Shape shape(256);
    shape.Build();
    //load data into a 3D texture
    glGenTextures(1, &_TextureShape);
    glBindTexture(GL_TEXTURE_3D, _TextureShape);
    // set the texture parameters
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, 256, 256, 256, 0, GL_RED, GL_UNSIGNED_BYTE, shape.GetData().data());
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_3D, 0);

    //Create gBuffer and texture to render to it
    glGenFramebuffers(1, &_gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
    // - albedo color buffer
    glGenTextures(1, &_gAlbedo);
    glBindTexture(GL_TEXTURE_2D, _gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gAlbedo, 0);
    // - normal color buffer
    glGenTextures(1, &_gNormal);
    glBindTexture(GL_TEXTURE_2D, _gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gNormal, 0);
    // - position color buffer
    glGenTextures(1, &_gPosition);
    glBindTexture(GL_TEXTURE_2D, _gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gPosition, 0);
    // - depth color buffer
    glGenTextures(1, &_gDepth);
    glBindTexture(GL_TEXTURE_2D, _gDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1920, 1080, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _gDepth, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Logger::Get()->Log(Logger::LogType::ERROR, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    _ImGuiLayer.Start(window);
}

void SimpleRenderer::StartFrame()
{
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

    glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

    unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_DEPTH_ATTACHMENT };
    glDrawBuffers(4, attachments);

    glClearColor(1.00f, 0.49f, 0.04f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _TextureShape);

    _Shader.Start();
    _Shader.LoadTime(glfwGetTime());
    GLint ViewportSize[4];
    glGetIntegerv(GL_VIEWPORT, ViewportSize);
    _Shader.LoadResolution(glm::vec2(ViewportSize[2], ViewportSize[3]));
    _Shader.LoadCameraViewMatrix(camera.GetViewMatrix());
    _Shader.LoadTextureVolume(0);

    glBindVertexArray(_EmptyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_3D, 0);

    _Shader.Stop();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}