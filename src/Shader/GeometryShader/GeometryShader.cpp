/*
** EPITECH PROJECT, 2021
** 3D
** File description:
** GeometryShader
*/

#include "GeometryShader.hpp"

GeometryShader::GeometryShader() : ShaderProgram("src/Shader/GeometryShader/vertexShader.glsl", "src/Shader/GeometryShader/fragmentShader.glsl")
{
    GetAllUniformLocation();
}

GeometryShader::~GeometryShader()
{
}

void GeometryShader::GetAllUniformLocation()
{
    loc_Time = GetUniformLocation("iTime");
    loc_Resolution = GetUniformLocation("iResolution");
    loc_cameraViewMatrix = GetUniformLocation("iViewMatrix");
    loc_transformMatrix = GetUniformLocation("iTransformMatrix");

    loc_textureVolume = GetUniformLocation("iVolume");
    loc_textureMaterial = GetUniformLocation("iMaterialPalette");
}

void GeometryShader::LoadTime(float time)
{
    LoadFloat(loc_Time, time);
}

void GeometryShader::LoadResolution(const glm::vec2 &resolution)
{
    LoadVector2(loc_Resolution, resolution);
}

void GeometryShader::LoadCameraViewMatrix(const glm::mat4 &viewMatrix)
{
    LoadMatrix(loc_cameraViewMatrix, viewMatrix);
}

void GeometryShader::LoadTransformMatrix(const glm::mat4 &transformMatrix)
{
    LoadMatrix(loc_transformMatrix, transformMatrix);
}

void GeometryShader::LoadTextureVolume(int texture)
{
    LoadInt(loc_textureVolume, texture);
}

void GeometryShader::LoadTextureMaterial(int texture)
{
    LoadInt(loc_textureMaterial, texture);
}