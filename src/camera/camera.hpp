

#ifndef CAMERA_CAMERA_HPP
#define CAMERA_CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

class Camera {
    private:
        float radius;
        float theta;
        float phi;
        glm::vec3 center;
        glm::vec3 look;
        glm::vec3 position;

        void update();

    public:
        Camera(float radius, float theta, float phi, glm::vec3 center = glm::vec3(0, 0, 0));
        ~Camera();
        glm::mat4 getViewMatrix();
        void updateTheta(float delta);
        void updatePhi(float delta);
        float getTheta();
        float getPhi();
};

#endif // CAMERA_CAMERA_HPP
