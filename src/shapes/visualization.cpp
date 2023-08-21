#include "shapes/visualization.hpp"

std::shared_ptr<Mesh> Visualization::halfCubeGrid(float size, unsigned int spaces) {
    std::vector<float> vertices;
    float points = spaces + 1;
    float offset = -(spaces/2.0)*size;

    for (float y = 0; y < spaces * size; y += size) {
        for (float x = 0; x < points * size; x += size) {
            // back plane
            {
                // vertical point 1
                vertices.push_back(x + offset);
                vertices.push_back(y + offset);
                vertices.push_back(offset);
                // vertical point 1
                vertices.push_back(x + offset);
                vertices.push_back(y + size + offset);
                vertices.push_back(offset);
                // horizontal point 1
                vertices.push_back(y + offset);
                vertices.push_back(x + offset);
                vertices.push_back(offset);
                // horizontal point 2
                vertices.push_back(y + size + offset);
                vertices.push_back(x + offset);
                vertices.push_back(offset);
            }
            // right plane
            {
                // vertical point 1
                vertices.push_back(-offset);
                vertices.push_back(y + offset);
                vertices.push_back(x + offset);
                // vertical point 1
                vertices.push_back(-offset);
                vertices.push_back(y + size + offset);
                vertices.push_back(x + offset);
                // horizontal point 1
                vertices.push_back(-offset);
                vertices.push_back(x + offset);
                vertices.push_back(y + offset);
                // horizontal point 2
                vertices.push_back(-offset);
                vertices.push_back(x + offset);
                vertices.push_back(y + size + offset);
            }
            // bottom plane
            {
                // vertical point 1
                vertices.push_back(x + offset);
                vertices.push_back(offset);
                vertices.push_back(y + offset);
                // vertical point 1
                vertices.push_back(x + offset);
                vertices.push_back(offset);
                vertices.push_back(y + size + offset);
                // horizontal point 1
                vertices.push_back(y + offset);
                vertices.push_back(offset);
                vertices.push_back(x + offset);
                // horizontal point 2
                vertices.push_back(y + size + offset);
                vertices.push_back(offset);
                vertices.push_back(x + offset);
            }
        }
    }

    std::shared_ptr<Mesh> grid = std::make_shared<Mesh>(vertices);
    grid->setDrawMode(GL_LINES);

    return grid;
}

std::shared_ptr<Mesh> Visualization::halfCube(float size) {
    // create vertices
    float w = size / 2.0f;
    std::vector<float> vertices = {
        -w, +w, -w - 0.01f,
        -w, -w, -w - 0.01f,
        +w, +w, -w - 0.01f,
        +w, +w, -w - 0.01f,
        -w, -w, -w - 0.01f,
        +w, -w, -w - 0.01f,
        +w + 0.01f, +w, -w,
        +w + 0.01f, -w, -w,
        +w + 0.01f, +w, +w,
        +w + 0.01f, -w, -w,
        +w + 0.01f, -w, +w,
        +w + 0.01f, +w, +w,
        -w, -w - 0.01f, -w,
        -w, -w - 0.01f, +w,
        +w, -w - 0.01f, -w,
        +w, -w - 0.01f, -w,
        -w, -w - 0.01f, +w,
        +w, -w - 0.01f, +w,
    };

    std::shared_ptr<Mesh> cube = std::make_shared<Mesh>(vertices);
    cube->setDrawMode(GL_TRIANGLES);

    return cube;
}
