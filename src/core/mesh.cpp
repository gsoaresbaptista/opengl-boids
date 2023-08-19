#include "core/mesh.hpp"
#include<iostream>

Mesh::Mesh(const std::vector<float>& vertices, const std::vector<GLuint>& indices)
    : vertices(vertices), indices(indices) {
    this->setup(true);
}

Mesh::Mesh(const std::vector<float>& vertices) : vertices(vertices) {
    this->setup(false);
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &VBO);
    if (this->EBO)
        glDeleteBuffers(1, &this->EBO);
    glDeleteVertexArrays(1, &this->VAO);
}

void Mesh::draw() {
    glBindVertexArray(this->VAO);

    // select draw function
    if (!this->EBO) {
        glDrawArrays(GL_LINE_LOOP, 0, vertices.size()/3);
    } else {
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
}

void Mesh::setup(bool withIndices) {
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    if (withIndices) {
        glGenBuffers(1, &this->EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}
