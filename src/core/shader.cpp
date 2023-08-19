#include "core/shader.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

// utils
std::string readShaderFile(const char* filePath) {
    std::ifstream shaderFile(filePath);
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    return shaderStream.str();
}

GLuint compileShader(GLenum shaderType, const char* source) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // check errors
    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(logLength, ' ');
        glGetShaderInfoLog(shader, logLength, &logLength, &log[0]);
        std::cerr << "Shader compilation error:" << std::endl << log << std::endl;
        return 0;
    }

    return shader;
}

// shader functions
Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    // read source files
    std::string vertexSource = readShaderFile(vertexPath);
    std::string fragmentSource = readShaderFile(fragmentPath);
    // compile
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource.c_str());
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str());
    // create a shader program
    this->ID = glCreateProgram();
    glAttachShader(this->ID, vertexShader);
    glAttachShader(this->ID, fragmentShader);
    glLinkProgram(this->ID);
    // remove desnecessary data
    glDetachShader(this->ID, vertexShader);
    glDetachShader(this->ID, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    if (this->ID) {
        glDeleteProgram(this->ID);
        this->ID = 0;
    }
}

void Shader::use() {
    glUseProgram(this->ID);
}

void Shader::uniform(const char* name, glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::uniform(const char* name, glm::vec3 &vec) {
    glUniform3fv(glGetUniformLocation(this->ID, name), 1, &vec[0]);
}

void Shader::uniform(const char* name, const float x, const float y, const float z) {
    glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}
