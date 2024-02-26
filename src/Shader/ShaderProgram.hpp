#pragma once

#ifndef SHADERPROGRAM_HPP_
#define SHADERPROGRAM_HPP_

#include "Include.hpp"

class ShaderProgram {
    public:
        ShaderProgram(const std::string &vertexFile, const std::string &fragmentFile);
        ~ShaderProgram();

        void Start() const;
        void Stop() const;
        void Destroy() const;

        void Reload();
    protected:
        int LoadShader(const std::string &file, unsigned int type) const;
        int LinkProgram(int vertexShaderID, int fragmentShaderID) const;
        void BindAttribute(int attribute, const std::string &name) const;

        int GetUniformLocation(const std::string &uniformName);
        virtual void GetAllUniformLocation() = 0;
        void LoadInt(int location, int value);
        void LoadFloat(int location, float value);
        void LoadBool(int location, bool value);
        void LoadVector2(int location, const glm::vec2 &value);
        void LoadVector3(int location, const glm::vec3 &value);
        void LoadMatrix(int location, const glm::mat4 &value);

    private:
        std::string _vertexFile;
        std::string _fragmentFile;

        int _programID;
        int _vertexShaderID;
        int _fragmentShaderID;
};

#endif /* !SHADERPROGRAM_HPP_ */
