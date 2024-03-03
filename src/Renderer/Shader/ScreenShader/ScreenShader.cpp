/*
** EPITECH PROJECT, 2021
** 3D
** File description:
** ScreenShader
*/

#include "ScreenShader.hpp"

ScreenShader::ScreenShader() : ShaderProgram("src/Renderer/Shader/ScreenShader/vertexShader.glsl", "src/Renderer/Shader/ScreenShader/fragmentShader.glsl")
{
    GetAllUniformLocation();
}

ScreenShader::~ScreenShader()
{
}

void ScreenShader::GetAllUniformLocation()
{
    loc_Time = GetUniformLocation("iTime");
    loc_Resolution = GetUniformLocation("iResolution");
    loc_ViewMatrix = GetUniformLocation("iViewMatrix");

    loc_textureAlbedo = GetUniformLocation("iAlbedo");
    loc_textureNormal = GetUniformLocation("iNormal");
    loc_texturePosition = GetUniformLocation("iPosition");
    loc_textureDepth = GetUniformLocation("iDepth");
}

void ScreenShader::LoadTime(float time)
{
    LoadFloat(loc_Time, time);
}

void ScreenShader::LoadResolution(const glm::vec2 &resolution)
{
    LoadVector2(loc_Resolution, resolution);
}

void ScreenShader::LoadCameraViewMatrix(const glm::mat4 &viewMatrix)
{
    LoadMatrix(loc_ViewMatrix, viewMatrix);
}

void ScreenShader::LoadTextureAlbedo(int texture)
{
    LoadInt(loc_textureAlbedo, texture);
}

void ScreenShader::LoadTextureNormal(int texture)
{
    LoadInt(loc_textureNormal, texture);
}

void ScreenShader::LoadTexturePosition(int texture)
{
    LoadInt(loc_texturePosition, texture);
}

void ScreenShader::LoadTextureDepth(int texture)
{
    LoadInt(loc_textureDepth, texture);
}