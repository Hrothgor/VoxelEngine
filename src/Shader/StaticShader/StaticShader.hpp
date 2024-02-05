#pragma once

#ifndef STATICSHADER_HPP_
#define STATICSHADER_HPP_

#include "Include.hpp"
#include "Shader/ShaderProgram.hpp"

class StaticShader : public ShaderProgram {
    public:
        StaticShader();
        ~StaticShader();

        void getAllUniformLocation();

        void loadTime(float time);
        void loadResolution(const glm::vec2 &resolution);

        void loadCameraViewMatrix(const glm::mat4 &viewMatrix);
 
    protected:
    private:
        int loc_Time;
        int loc_Resolution;

        int loc_cameraViewMatrix;
};

#endif /* !STATICSHADER_HPP_ */
