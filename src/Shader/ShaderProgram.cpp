#include "ShaderProgram.hpp"
#include <fstream>
#include <iostream>

ShaderProgram::ShaderProgram(std::string vertexFile, std::string fragmentFile)
{
    _vertexShaderID = loadShader(vertexFile, GL_VERTEX_SHADER);
    _fragmentShaderID = loadShader(fragmentFile, GL_FRAGMENT_SHADER);
    _programID = glCreateProgram();
    glAttachShader(_programID, _vertexShaderID);
    glAttachShader(_programID, _fragmentShaderID);
    glLinkProgram(_programID);
    glValidateProgram(_programID);
}

ShaderProgram::~ShaderProgram()
{
}

void ShaderProgram::start() const
{
    glUseProgram(_programID);
}

void ShaderProgram::stop() const
{
    glUseProgram(0);
}

void ShaderProgram::destroy() const
{
    stop();
    glDetachShader(_programID, _vertexShaderID);
    glDetachShader(_programID, _fragmentShaderID);
    glDeleteShader(_vertexShaderID);
    glDeleteShader(_fragmentShaderID);
    glDeleteProgram(_programID);
}

int ShaderProgram::loadShader(const std::string &file, unsigned int type) const
{
    std::ifstream fs(file);

    if (!fs.is_open()) {
        std::cout << "Couldn't open " << file << " shader file" << std::endl;
        exit(84);
    }
    std::string source((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
    const GLchar *const src = source.c_str();
    GLuint shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &src, NULL);
    glCompileShader(shaderID);
    GLint succes;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &succes);
    GLchar infolog[512];
    if (!succes) {
        glGetShaderInfoLog(shaderID, 512, NULL, infolog);
        std::cout << "Could not compile shader : " << file << " : " << infolog << std::endl;
    }
    return (shaderID);
}

void ShaderProgram::bindAttribute(int attribute, const std::string &name) const
{
    glBindAttribLocation(_programID, attribute, name.c_str());
}

int ShaderProgram::getUniformLocation(const std::string &uniformName)
{
    return (glGetUniformLocation(_programID, uniformName.c_str()));
}

void ShaderProgram::loadFloat(int location, float value)
{
    glUniform1f(location, value);
}

void ShaderProgram::loadVector2(int location,const glm::vec2 &value)
{
    glUniform2f(location, value.x, value.y);
}

void ShaderProgram::loadVector3(int location, const glm::vec3 &value)
{
    glUniform3f(location, value.x, value.y, value.z);
}

void ShaderProgram::loadBool(int location, bool value)
{
    if (value == true)
        glUniform1f(location, 1);
    else
        glUniform1f(location, 0);
}

void ShaderProgram::loadMatrix(int location, const glm::mat4 &value)
{
    glUniformMatrix4fv(location, 1, false, &value[0][0]);
}