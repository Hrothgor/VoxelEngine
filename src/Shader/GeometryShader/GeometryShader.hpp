#pragma once

#ifndef GEOMETRYSHADER_HPP_
#define GEOMETRYSHADER_HPP_

#include "Include.hpp"
#include "Shader/ShaderProgram.hpp"

class GeometryShader : public ShaderProgram {
    public:
        GeometryShader();
        ~GeometryShader();

        void LoadTime(float time);
        void LoadResolution(const glm::vec2 &resolution);
        void LoadCameraViewMatrix(const glm::mat4 &viewMatrix);
        void LoadTransformMatrix(const glm::mat4 &transformMatrix);

        void LoadTextureVolume(int texture);
        void LoadTextureMaterial(int texture);
    protected:
        void GetAllUniformLocation();

    private:
        int loc_Time;
        int loc_Resolution;
        int loc_cameraViewMatrix;
        int loc_transformMatrix;

        int loc_textureVolume;
        int loc_textureMaterial; 
};

#endif /* !GEOMETRYSHADER_HPP_ */
