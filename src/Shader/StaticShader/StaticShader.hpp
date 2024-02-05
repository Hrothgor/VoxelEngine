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

        void loadTransformationMatrix(const glm::mat4 &matrix);
        void loadProjectionMatrix(const glm::mat4 &matrix);
        void loadViewMatrix(const glm::mat4 &matrix);

        void loadTime(float time);
        void loadResolution(const glm::vec2 &resolution);

        void loadCamera(const glm::vec3 &position, const glm::mat4 &rotation);
 
    protected:
    private:
        int loc_Time;
        int loc_Resolution;

        int loc_cameraPosition;
        int loc_cameraRotation;
};

#endif /* !STATICSHADER_HPP_ */
