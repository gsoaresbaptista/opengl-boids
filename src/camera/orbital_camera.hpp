

#ifndef CAMERA_ORBITAL_CAMERA_HPP
#define CAMERA_ORBITAL_CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

class OrbitalCamera {
    private:
        float radius;
        float theta;
        float phi;
        glm::vec3 center;
        glm::vec3 look;
        glm::vec3 position;

        void update();

    public:
        OrbitalCamera(float radius, float theta, float phi, glm::vec3 center = glm::vec3(0, 0, 0));
        ~OrbitalCamera();
        glm::mat4 getViewMatrix();
        void updateTheta(float delta);
        void updatePhi(float delta);
        void updateRadius(float delta);
        float getTheta();
        float getPhi();
};

#endif // CAMERA_ORBITAL_CAMERA_HPP
