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

    _Shader.loadCamera(camera.GetPosition(), camera.GetForward());

    glBindVertexArray(_EmptyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    _Shader.stop();
}

glm::mat4 SimpleRenderer::createTransformationMatrix(glm::vec3 translation, glm::vec3 rotation, float scale)
{
    glm::mat4 matrix(1.0f);

    matrix = glm::translate(matrix, translation);
    matrix = glm::rotate(matrix, rotation.x, glm::vec3(1, 0, 0));
    matrix = glm::rotate(matrix, rotation.y, glm::vec3(0, 1, 0));
    matrix = glm::rotate(matrix, rotation.z, glm::vec3(0, 0, 1));
    matrix = glm::scale(matrix, glm::vec3(scale, scale, scale));
    return (matrix);
}

glm::mat4 SimpleRenderer::createProjectionMatrix()
{
    glm::mat4 matrix(1.0f);


    float aspectRatio = WIDTH / HEIGHT;
    float y_scale = (1.0 / tanf((FOV / 2.0) * (M_PI / 180)) * aspectRatio);
    float x_scale = y_scale / aspectRatio;
    float frustum_length = FAR_PLANE - NEAR_PLANE;

    matrix[0][0] = x_scale;
    matrix[1][1] = y_scale;
    matrix[2][2] = -((FAR_PLANE + NEAR_PLANE) / frustum_length);
    matrix[2][3] = -1;
    matrix[3][2] = -((2 * NEAR_PLANE * FAR_PLANE) / frustum_length);
    matrix[3][3] = 0;
    return (matrix);
}

glm::mat4 SimpleRenderer::createViewMatrix(glm::vec3 pos, float pitch, float yaw, float roll)
{
    glm::mat4 matrix(1.0f);

    matrix = glm::rotate(matrix, pitch, glm::vec3(1, 0, 0));
    matrix = glm::rotate(matrix, yaw, glm::vec3(0, 1, 0));
    matrix = glm::rotate(matrix, roll, glm::vec3(0, 0, 1));
    matrix = glm::translate(matrix, glm::vec3(-pos.x, -pos.y, -pos.z));
    return (matrix);
}