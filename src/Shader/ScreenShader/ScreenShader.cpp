/*
** EPITECH PROJECT, 2021
** 3D
** File description:
** ScreenShader
*/

#include "ScreenShader.hpp"

ScreenShader::ScreenShader() : ShaderProgram("src/Shader/ScreenShader/vertexShader.glsl", "src/Shader/ScreenShader/fragmentShader.glsl")
{
    GetAllUniformLocation();
}

ScreenShader::~ScreenShader()
{
}

void ScreenShader::GetAllUniformLocation()
{
    loc_Resolution = GetUniformLocation("iResolution");
    loc_textureAlbedo = GetUniformLocation("iAlbedo");
    loc_textureNormal = GetUniformLocation("iNormal");
    loc_texturePosition = GetUniformLocation("iPosition");
    loc_textureDepth = GetUniformLocation("iDepth");
}

void ScreenShader::LoadResolution(const glm::vec2 &resolution)
{
    LoadVector2(loc_Resolution, resolution);
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