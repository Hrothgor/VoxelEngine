#pragma once

#ifndef STATICSHADER_HPP_
#define STATICSHADER_HPP_

#include "Include.hpp"
#include "Shader/ShaderProgram.hpp"

class StaticShader : public ShaderProgram {
    public:
        StaticShader();
        ~StaticShader();

        void LoadTime(float time);
        void LoadResolution(const glm::vec2 &resolution);
        void LoadCameraViewMatrix(const glm::mat4 &viewMatrix);
        void LoadTextureVolume(int texture);
 
    protected:
        void GetAllUniformLocation();

    private:
        int loc_Time;
        int loc_Resolution;
        int loc_cameraViewMatrix;
        int loc_textureVolume;
};

#endif /* !STATICSHADER_HPP_ */
