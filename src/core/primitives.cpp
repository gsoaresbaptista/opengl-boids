#include "core/primitives.hpp"

#include <cmath>

std::vector<float> Primitives::circle(float radius, unsigned int points) {
    float deltaTheta = M_PI*2/points;
    std::vector<float> circle;

    for (float theta = 0; theta < 2*M_PI; theta += deltaTheta) {
        circle.push_back(radius * cos(theta));
        circle.push_back(radius * sin(theta));
        circle.push_back(0);
    }

    return circle;
}
