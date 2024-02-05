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

    loc_cameraViewMatrix = getUniformLocation("iViewMatrix");
}

void StaticShader::loadTime(float time)
{
    loadFloat(loc_Time, time);
}

void StaticShader::loadResolution(const glm::vec2 &resolution)
{
    loadVector2(loc_Resolution, resolution);
}

void StaticShader::loadCameraViewMatrix(const glm::mat4 &viewMatrix)
{
    loadMatrix(loc_cameraViewMatrix, viewMatrix);
}