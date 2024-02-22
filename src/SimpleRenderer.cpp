#include "SimpleRenderer.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <chrono>
#include "SVO.hpp"
#include "Engine.hpp"  
#include "Logger.hpp" 

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

    //Create FrameBuffer and texture to render to it
    glGenFramebuffers(1, &_FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _FrameBuffer);
    glViewport(0, 0, 1920 / 2, 1080 / 2);

    glGenTextures(1, &_Texture);
    glBindTexture(GL_TEXTURE_2D, _Texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920 / 2, 1080 / 2, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _Texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Logger::Get()->Log(Logger::LogType::ERROR, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto start = std::chrono::high_resolution_clock::now();
    {
        int size = 256;
        std::vector<float> noiseOutput(size * size);

        auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
        fnSimplex->GenUniformGrid2D(noiseOutput.data(), 0, 0, size, size, 0.01, rand());
        
        SVO svo(log(size) / log(2));
        svo.Build(noiseOutput);

        // Create buffer
        glGenBuffers(1, &_SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, _SSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _SSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, svo.GetOctree().size() * sizeof(SVO::Node), svo.GetOctree().data(), GL_STATIC_READ);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    Logger::Get()->Log(Logger::LogType::INFO, "Time to build SVO with noise: %.3f ms", std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(finish - start).count());

    _ImGuiLayer.Start(window);
}

void SimpleRenderer::StartFrame()
{
}

void SimpleRenderer::EndFrame()
{
}

void SimpleRenderer::DrawFullScreenTriangle(const Camera &camera)
{
    _ImGuiLayer.BeginFrame();
    _ImGuiLayer.ImGuiRender();
    _ImGuiLayer.EndFrame();

    glBindFramebuffer(GL_FRAMEBUFFER, _FrameBuffer);
    glViewport(0, 0, 1920 / 2, 1080 / 2);

    glClearColor(1.00f, 0.49f, 0.04f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _Shader.Start();
    _Shader.LoadTime(glfwGetTime());
    GLint ViewportSize[4];
    glGetIntegerv(GL_VIEWPORT, ViewportSize);
    _Shader.LoadResolution(glm::vec2(ViewportSize[2], ViewportSize[3]));
    _Shader.LoadCameraViewMatrix(camera.GetViewMatrix());

    glBindVertexArray(_EmptyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    _Shader.Stop();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}