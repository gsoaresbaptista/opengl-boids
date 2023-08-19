#ifndef CORE_SHADER_HPP_
#define CORE_SHADER_HPP_

#include "glad/glad.h"

#include "glm/glm.hpp"

class Shader {
    private:
        GLuint ID = 0;

    public:
        Shader(const char* vertexPath, const char* fragmentPath);
        ~Shader();
        void use();
        void uniform(const char* name, glm::mat4& mat);
        void uniform(const char* name, glm::vec3 &vec);
        void uniform(const char* name, const float x, const float y, const float z);
};

#endif  //CORE_SHADER_HPP_
