#include "shapes/primitives.hpp"

#include <cmath>

std::shared_ptr<Mesh> Primitives::circle(float radius, unsigned int points) {
    float deltaTheta = M_PI*2/points;
    std::vector<float> vertices;

    for (float theta = 0; theta < 2*M_PI; theta += deltaTheta) {
        vertices.push_back(radius * cos(theta));
        vertices.push_back(radius * sin(theta));
        vertices.push_back(0);
    }

    return std::make_shared<Mesh>(vertices);
}
