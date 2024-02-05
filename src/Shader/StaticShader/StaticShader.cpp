/*
** EPITECH PROJECT, 2021
** 3D
** File description:
** StaticShader
*/

#include "StaticShader.hpp"

StaticShader::StaticShader() : ShaderProgram("src/Shader/StaticShader/vertexShader.glsl", "src/Shader/StaticShader/fragmentShader.glsl")
{
    getAllUniformLocation();
}

StaticShader::~StaticShader()
{
}

void StaticShader::getAllUniformLocation()
{
    loc_Time = getUniformLocation("iTime");
    loc_Resolution = getUniformLocation("iResolution");

    loc_cameraPosition = getUniformLocation("iCamPos");
    loc_cameraRotation = getUniformLocation("iViewMatrix");
}

void StaticShader::loadTransformationMatrix(const glm::mat4 &matrix)
{
    // loadMatrix(loc_transformationMatrix, matrix);
}

void StaticShader::loadProjectionMatrix(const glm::mat4 &matrix)
{
    // loadMatrix(loc_projectionMatrix, matrix);
}

void StaticShader::loadViewMatrix(const glm::mat4 &matrix)
{
    // loadMatrix(loc_viewMatrix, matrix);
}

void StaticShader::loadTime(float time)
{
    loadFloat(loc_Time, time);
}

void StaticShader::loadResolution(const glm::vec2 &resolution)
{
    loadVector2(loc_Resolution, resolution);
}

void StaticShader::loadCamera(const glm::vec3 &position, const glm::mat4 &rotation)
{
    loadVector3(loc_cameraPosition, position);
    loadMatrix(loc_cameraRotation, rotation);
}