#include "orbital_camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

const glm::vec3 GLOBAL_UP(0, 1, 0);

OrbitalCamera::OrbitalCamera(float radius, float theta, float phi, glm::vec3 center)
    : radius(radius), theta(theta), phi(phi), center(center) {
    // compute first position
    this->update();
}

OrbitalCamera::~OrbitalCamera() {
}

glm::mat4 OrbitalCamera::getViewMatrix() {
    return glm::lookAt(this->position, this->look, GLOBAL_UP);
}

void OrbitalCamera::updateTheta(float delta) {
    this->theta = fmodf(this->theta - delta, 360.0);
    this->update();
}

void OrbitalCamera::updateRadius(float delta) {
    this->radius -= delta;
    this->update();
}


void OrbitalCamera::updatePhi(float delta){
    this->phi -= delta;
    if (this->phi < 0.1f) this->phi = 0.1f;
    if (this->phi > 90.1f) this->phi = 90.1f;
    this->update();
}

float OrbitalCamera::getTheta() {
    if (this->theta > 0) {
        return this->theta - 360;
    } else {
        return this->theta;
    }
}

float OrbitalCamera::getPhi() {
    return this->phi;
}

void OrbitalCamera::update() {
    // compute euler coordinates
    float tmp = radius*sinf(phi * M_PI/180.f);
    float x = tmp*sinf(theta * M_PI/180.f) + center.x;
    float y = radius * cosf(phi * M_PI/180.f) + center.y;
    float z = tmp*cosf(theta * M_PI/180.f) + center.z;

    // update members
    this->position = glm::vec3(x, y, z);
    this->look = this->position * (-1.f);
}
