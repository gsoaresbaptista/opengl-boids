#ifndef CORE_MESH_HPP_
#define CORE_MESH_HPP_

#include "glad/glad.h"

#include <vector>

class Mesh {
    protected:
        std::vector<float> vertices;
        std::vector<GLuint> indices;
        GLuint drawMode = GL_LINE_LOOP;
        GLuint VAO = 0, VBO = 0, EBO = 0;

    public:
        Mesh(const std::vector<float>& vertices, const std::vector<GLuint>& indices);
        Mesh(const std::vector<float>& vertices);
        ~Mesh();
        void setup(bool withIndices);
        void draw();
        void setDrawMode(GLuint mode);
};

#endif  // CORE_MESH_HPP_
