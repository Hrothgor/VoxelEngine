/*
** EPITECH PROJECT, 2021
** 3D
** File description:
** StaticShader
*/

#include "StaticShader.hpp"

StaticShader::StaticShader() : ShaderProgram("src/Shader/StaticShader/vertexShader.glsl", "src/Shader/StaticShader/fragmentShader.glsl")
{
    GetAllUniformLocation();
}

StaticShader::~StaticShader()
{
}

void StaticShader::GetAllUniformLocation()
{
    loc_Time = GetUniformLocation("iTime");
    loc_Resolution = GetUniformLocation("iResolution");
    loc_cameraViewMatrix = GetUniformLocation("iViewMatrix");
    loc_textureVolume = GetUniformLocation("iVolume");
}

void StaticShader::LoadTime(float time)
{
    LoadFloat(loc_Time, time);
}

void StaticShader::LoadResolution(const glm::vec2 &resolution)
{
    LoadVector2(loc_Resolution, resolution);
}

void StaticShader::LoadCameraViewMatrix(const glm::mat4 &viewMatrix)
{
    LoadMatrix(loc_cameraViewMatrix, viewMatrix);
}

void StaticShader::LoadTextureVolume(int texture)
{
    LoadInt(loc_textureVolume, texture);
}