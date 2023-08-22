#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "core/shader.hpp"
#include "core/mesh.hpp"
#include "shapes/primitives.hpp"
#include "shapes/visualization.hpp"
#include "camera/camera.hpp"

#include <vector>
#include <memory>
#include <iostream>
#include <random>
#include <cmath>

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// global settings
unsigned int windowWidth = 1280;
unsigned int windowHeight = 720;
const unsigned int seed = 42;

// camera settings
Camera camera(100, 0.f, 90.f, glm::vec3(0.f, 0.f, 0.f));

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (window) {
        static float lastMousePosx = 0.f;
        static float lastMousePosy = 0.f;
        float dx, dy;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            dx = xpos - lastMousePosx;
            if (fabs(dx) > 10.f) dx = 10.f;
            camera.updateTheta(dx);
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            dy = ypos - lastMousePosy;
            if (fabs(dy) > 10.f) dy = 10.f;
            camera.updatePhi(dy);
        }

        lastMousePosx = xpos;
        lastMousePosy = ypos;
    }
}

void key_callback(
        GLFWwindow* window,
        int key, int scancode __attribute__((unused)),
        int action, int mods __attribute__((unused))
    ) {
    // close window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

void updateBoids(std::vector<glm::vec3> &boidPositions, std::vector<glm::vec3> &boidVelocities, float dt) {
    for (unsigned int i = 0; i < boidPositions.size(); i++) {
        boidPositions[i] += boidVelocities[i] * dt;
        // check if boids go out of the cube, if so wrap them to the other side
        if (boidPositions[i].x < -25.f) boidPositions[i].x = 25.f;
        if (boidPositions[i].y < -25.f) boidPositions[i].y = 25.f;
        if (boidPositions[i].z < -25.f) boidPositions[i].z = 25.f;
        if (boidPositions[i].x > +25.f) boidPositions[i].x = -25.f;
        if (boidPositions[i].y > +25.f) boidPositions[i].y = -25.f;
        if (boidPositions[i].z > +25.f) boidPositions[i].z = -25.f;
    }
}

int main() {
    // set opengl context
    assert(glfwInit());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // creating window
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL - Boids", NULL, NULL);
    assert(window);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // load glad
    assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

    // opengl settings
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.85f, 0.85f, 0.85f, 1.0f);

    // opengl scope
    {
        // bird data
        std::vector<float> vertices = {
            0.3536f, 0.0f, 0.0f,
            -0.15f, 0.0f, 0.05f,
            -0.25f, 0.25f, 0.0f,
            -0.25f, -0.25f, 0.0f,
            -0.15f, 0.0f, -0.05f,
        };
        std::vector<GLuint> indices = {
            0, 2, 1,
            0, 1, 3,
            1, 2, 4,
            1, 4, 3,
            4, 2, 0,
            0, 3, 4,
        };

        Shader shader("resources/shaders/main.vs", "resources/shaders/main.fs");
        Mesh bird(vertices, indices);

        // get grid points
        int grids = 50;
        float space = 1.0;
        std::shared_ptr<Mesh> grid = Visualization::halfCubeGrid(space, grids);
        std::shared_ptr<Mesh> cube = Visualization::halfCube(grids * space);

        // perspective matrices
        glm::mat4 model = glm::mat4(1.0);
        glm::mat4 projection = glm::perspective(glm::radians(40.0f), (float)windowWidth / windowHeight, 0.1f, 500.0f);

        // get circle points
        std::shared_ptr<Mesh> circle = Primitives::circle(0.3536, 30);
        std::shared_ptr<Mesh> neighborhood = Primitives::circle(0.3536*5, 100);

        // algorithm
        bool drawCollisionRegion = false;
        bool drawNeighborhood = false;

        // generate random boids
        int nBoids = 50;
        float w = grids * space / 2.0;
        std::vector<glm::vec3> boidPositions;
        std::vector<glm::vec3> boidVelocities;

        // random generator
        std::mt19937 generator(seed);
        std::uniform_real_distribution<float> position(-w + 0.6, +w - 0.6);
        std::uniform_real_distribution<float> velocity(-3.0, 3.0);

        for (int i = 0; i < nBoids; i++) {
            boidPositions.push_back(glm::vec3(
                position(generator),
                position(generator),
                position(generator)
            ));
            boidVelocities.push_back(glm::vec3(
                velocity(generator),
                velocity(generator),
                velocity(generator)
            ));
        }

        while (!glfwWindowShouldClose(window)) {
            float currentFrame = static_cast<float>(glfwGetTime()/2);
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            updateBoids(boidPositions, boidVelocities, deltaTime);
            glm::mat4 view = camera.getViewMatrix();

            // set uniforms
            shader.use();
            shader.uniform("projection", projection);
            shader.uniform("view", view);

            // render phase
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // draw grid
            shader.uniform("model", model);
            shader.uniform("color", 0.8f, 0.8f, 0.85f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            cube->draw();
            shader.uniform("model", model);
            shader.uniform("color", 0.f, 0.f, 0.f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            grid->draw();

            for (unsigned int i = 0; i < boidPositions.size(); i++) {
                glm::mat4 rotated, tmp_model = glm::translate(model, boidPositions[i]);

                if (drawCollisionRegion) {
                    shader.uniform("color", 0.2f, 0.2f, 0.2f);
                    circle->draw();
                    rotated = glm::rotate(tmp_model, (float)M_PI/2, glm::vec3(1, 0, 0));
                    shader.uniform("model", rotated);
                    circle->draw();
                    rotated = glm::rotate(tmp_model, (float)M_PI/2, glm::vec3(0, 1, 0));
                    shader.uniform("model", rotated);
                    circle->draw();
                    rotated = glm::rotate(tmp_model, (float)M_PI/2, glm::vec3(0, 0, 1));
                    shader.uniform("model", rotated);
                    circle->draw();
                }

                if (drawNeighborhood) {
                    shader.uniform("color", 0.75f, 0.75f, 0.75f);
                    rotated = glm::rotate(tmp_model, (float)M_PI/2, glm::vec3(1, 0, 0));
                    shader.uniform("model", rotated);
                    neighborhood->draw();
                    rotated = glm::rotate(tmp_model, (float)M_PI/2, glm::vec3(0, 1, 0));
                    shader.uniform("model", rotated);
                    neighborhood->draw();
                    rotated = glm::rotate(tmp_model, (float)M_PI/2, glm::vec3(0, 0, 1));
                    shader.uniform("model", rotated);
                    neighborhood->draw();
                }

                // fix bird rotation
                glm::vec3 v = boidVelocities[i];
                tmp_model = glm::rotate(tmp_model, -atan2f(v.z, v.x), glm::vec3(0, 1, 0));
                tmp_model = glm::rotate(tmp_model, atan2f(v.y, sqrtf(v.x*v.x+v.z*v.z)), glm::vec3(0, 0, 1));
                shader.uniform("model", tmp_model);
                shader.uniform("color", 0.f, 0.f, 0.f);
                bird.draw();
            }

            // process screen
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    // glfw terminate
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
