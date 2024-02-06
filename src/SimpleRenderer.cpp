#include "SimpleRenderer.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <chrono>
#include "SVO.hpp"

#define WIDTH 800.0
#define HEIGHT 600.0
#define FOV 70.0
#define NEAR_PLANE 0.1
#define FAR_PLANE 1000.0

SimpleRenderer::SimpleRenderer()
{
    glGenVertexArrays(1, &_EmptyVAO);

    auto start = std::chrono::high_resolution_clock::now();
    {
        int size = 8;
        std::vector<float> noiseOutput(size * size);
        auto fnSimplex = FastNoise::New<FastNoise::Value>();
        fnSimplex->GenUniformGrid2D(noiseOutput.data(), 0, 0, size, size, 0.2, 1337);
        
        SVO svo(log(size) / log(2));
        svo.Build(noiseOutput);

        // Test octrre with bottom filled and one top filled
        std::vector<SVO::Node> octree;
        // full octree
        octree.push_back({0, {1, 2, 3, 4, 5, 6, 7, 8}});

        octree.push_back({2147483649, {0, 0, 0, 0, 0, 0, 0, 0}});
        octree.push_back({2147483649, {0, 0, 0, 0, 0, 0, 0, 0}});
        octree.push_back({2147483649, {0, 0, 0, 0, 0, 0, 0, 0}});
        octree.push_back({2147483649, {0, 0, 0, 0, 0, 0, 0, 0}});

        octree.push_back({2147483648, {0, 0, 0, 0, 0, 0, 0, 0}});
        octree.push_back({2147483648, {0, 0, 0, 0, 0, 0, 0, 0}});
        octree.push_back({2147483648, {0, 0, 0, 0, 0, 0, 0, 0}});
        octree.push_back({2147483649, {0, 0, 0, 0, 0, 0, 0, 0}});


        // Create buffer
        glGenBuffers(1, &_SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, _SSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, octree.size() * sizeof(SVO::Node), octree.data(), GL_STATIC_READ);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "Time to build: " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(finish - start).count() << "ms" << std::endl << std::endl;
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

    _Shader.loadCameraViewMatrix(camera.GetViewMatrix());

    glBindVertexArray(_EmptyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    _Shader.stop();
}