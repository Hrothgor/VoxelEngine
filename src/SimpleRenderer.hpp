#pragma once

#ifndef SIMPLERENDERER_HPP_
#define SIMPLERENDERER_HPP_

#include "Include.hpp"
#include "Shader/StaticShader/StaticShader.hpp"
#include "Camera.hpp"

class SimpleRenderer {
public:
    SimpleRenderer();
    ~SimpleRenderer();

    void Init();

    void StartFrame();
    void EndFrame();
    void DrawFullScreenTriangle(const Camera &camera);

private:
    glm::mat4 createTransformationMatrix(glm::vec3 translation, glm::vec3 rotation, float scale);
    glm::mat4 createProjectionMatrix();
    glm::mat4 createViewMatrix(glm::vec3 pos, float pitch, float yaw, float roll);

    StaticShader _Shader;
    GLuint _EmptyVAO;
};


#endif /* !SIMPLERENDERER_HPP_ */