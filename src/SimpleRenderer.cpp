#include "SimpleRenderer.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <chrono>

#define WIDTH 800.0
#define HEIGHT 600.0
#define FOV 70.0
#define NEAR_PLANE 0.1
#define FAR_PLANE 1000.0

SimpleRenderer::SimpleRenderer()
{
    glGenVertexArrays(1, &_EmptyVAO);
}

SimpleRenderer::~SimpleRenderer()
{
    _Shader.destroy();
}

void SimpleRenderer::StartFrame()
{
    glClearColor(1.00f, 0.49f, 0.04f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SimpleRenderer::EndFrame()
{

}

void SimpleRenderer::DrawFullScreenTriangle(const Camera &camera)
{
    _Shader.start();

    _Shader.loadTime(glfwGetTime());
    GLint ViewportSize[4];

    glGetIntegerv(GL_VIEWPORT, ViewportSize);
    _Shader.loadResolution(glm::vec2(ViewportSize[2], ViewportSize[3]));

    _Shader.loadCamera(camera.GetPosition(), camera.GetViewMatrix());

    glBindVertexArray(_EmptyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    _Shader.stop();
}