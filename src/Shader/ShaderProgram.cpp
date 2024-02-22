#include "ShaderProgram.hpp"
#include <fstream>
#include <iostream>
#include "Logger.hpp"

ShaderProgram::ShaderProgram(const std::string &vertexFile, const std::string &fragmentFile)
{
    _vertexFile = vertexFile;
    _fragmentFile = fragmentFile;

    _vertexShaderID = LoadShader(_vertexFile, GL_VERTEX_SHADER);
    _fragmentShaderID = LoadShader(_fragmentFile, GL_FRAGMENT_SHADER);
    _programID = LinkProgram(_vertexShaderID, _fragmentShaderID);
}

ShaderProgram::~ShaderProgram()
{
}

void ShaderProgram::Start() const
{
    glUseProgram(_programID);
}

void ShaderProgram::Stop() const
{
    glUseProgram(0);
}

void ShaderProgram::Destroy() const
{
    Stop();
    glDetachShader(_programID, _vertexShaderID);
    glDetachShader(_programID, _fragmentShaderID);
    glDeleteShader(_vertexShaderID);
    glDeleteShader(_fragmentShaderID);
    glDeleteProgram(_programID);
}

int ShaderProgram::LoadShader(const std::string &file, unsigned int type) const
{
    std::ifstream fs(file);

    if (!fs.is_open()) {
        Logger::Get()->Log(Logger::LogType::ERROR, "Couldn't open %s shader file", file.c_str());
        return (0);
    }
    std::string source((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
    const GLchar *const src = source.c_str();
    GLuint shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &src, NULL);
    glCompileShader(shaderID);

    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    GLchar infolog[512];
    if (!success) {
        glGetShaderInfoLog(shaderID, 512, NULL, infolog);
        Logger::Get()->Log(Logger::LogType::ERROR, "Could not compile shader : %s", file.c_str());
        Logger::Get()->Log(Logger::LogType::ERROR, "\t%s", infolog);
        return (0);
    }
    return (shaderID);
}

int ShaderProgram::LinkProgram(int vertexShaderID, int fragmentShaderID) const
{
    if (vertexShaderID == 0 || fragmentShaderID == 0) {
        Logger::Get()->Log(Logger::LogType::ERROR, "Can't link shader program, one of the shader is invalid");
        return (0);
    }
    
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    glValidateProgram(programID);

    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    GLchar infolog[512];
    if (!success) {
        glGetProgramInfoLog(programID, 512, NULL, infolog);
        Logger::Get()->Log(Logger::LogType::ERROR, "Could not validate shader program");
        Logger::Get()->Log(Logger::LogType::ERROR, "\t%s", infolog);
        return (0);
    }
    return (programID);
}

void ShaderProgram::Reload()
{
    GLuint vertexShaderID = LoadShader(_vertexFile, GL_VERTEX_SHADER);
    GLuint fragmentShaderID = LoadShader(_fragmentFile, GL_FRAGMENT_SHADER);
    if (vertexShaderID == 0 || fragmentShaderID == 0) {
        Logger::Get()->Log(Logger::LogType::WARNING, "Couldn't reload shader");
        return;
    }

    GLuint programID = LinkProgram(vertexShaderID, fragmentShaderID);
    if (programID == 0)
    {
        Logger::Get()->Log(Logger::LogType::WARNING, "Couldn't reload shader");
        return;
    }

    Stop();
    _vertexShaderID = vertexShaderID;
    _fragmentShaderID = fragmentShaderID;
    _programID = programID;
    Logger::Get()->Log(Logger::LogType::SUCCESS, "Shader reloaded");
}

void ShaderProgram::BindAttribute(int attribute, const std::string &name) const
{
    glBindAttribLocation(_programID, attribute, name.c_str());
}

int ShaderProgram::GetUniformLocation(const std::string &uniformName)
{
    return (glGetUniformLocation(_programID, uniformName.c_str()));
}

void ShaderProgram::LoadFloat(int location, float value)
{
    glUniform1f(location, value);
}

void ShaderProgram::LoadVector2(int location,const glm::vec2 &value)
{
    glUniform2f(location, value.x, value.y);
}

void ShaderProgram::LoadVector3(int location, const glm::vec3 &value)
{
    glUniform3f(location, value.x, value.y, value.z);
}

void ShaderProgram::LoadBool(int location, bool value)
{
    if (value == true)
        glUniform1f(location, 1);
    else
        glUniform1f(location, 0);
}

void ShaderProgram::LoadMatrix(int location, const glm::mat4 &value)
{
    glUniformMatrix4fv(location, 1, false, &value[0][0]);
}