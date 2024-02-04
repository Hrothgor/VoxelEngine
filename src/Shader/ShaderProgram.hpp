#pragma once

#ifndef SHADERPROGRAM_HPP_
#define SHADERPROGRAM_HPP_

#include "Include.hpp"

class ShaderProgram {
    public:
        ShaderProgram(std::string vertexFile, std::string fragmentFile);
        ~ShaderProgram();

        void start() const;
        void stop() const;
        void destroy() const;
        int loadShader(const std::string &file, unsigned int type) const;
        void bindAttribute(int attribute, const std::string &name) const;

        int getUniformLocation(const std::string &uniformName);
        virtual void getAllUniformLocation() = 0;
        void loadFloat(int location, float value);
        void loadBool(int location, bool value);
        void loadVector2(int location, const glm::vec2 &value);
        void loadVector3(int location, const glm::vec3 &value);
        void loadMatrix(int location, const glm::mat4 &value);

    protected:
    private:
        int _programID;
        int _vertexShaderID;
        int _fragmentShaderID;
};

#endif /* !SHADERPROGRAM_HPP_ */
