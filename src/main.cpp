#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "core/shader.hpp"
#include "core/mesh.hpp"
#include "shapes/primitives.hpp"
#include "shapes/visualization.hpp"

#include <vector>
#include <memory>
#include <iostream>
#include <random>

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// visualization
float boxAngle = 45.0f;
float globalAngle = 45.0f;

// global settings
unsigned int windowWidth = 1280;
unsigned int windowHeight = 720;
const unsigned int seed = 42;

void key_callback(
        GLFWwindow* window,
        int key, int scancode __attribute__((unused)),
        int action, int mods __attribute__((unused))
    ) {
    // close window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
    // update angle
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        globalAngle -= 1.0;
        boxAngle -= 1.0;
    }
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        globalAngle += 1.0;
        boxAngle += 1.0;
    }
    // fix box angle
    if (boxAngle > 90 && boxAngle < 180)
        boxAngle -= 90;
    if (boxAngle < 0)
        boxAngle += 90;
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
        int grids = 20;
        float space = 1.0;
        std::shared_ptr<Mesh> grid = Visualization::halfCubeGrid(space, grids + 2);
        std::shared_ptr<Mesh> cube = Visualization::halfCube((grids + 2) * space);

        // perspective matrices
        glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, -2*grids*space));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / windowHeight, 0.1f, 100.0f);
        glm::mat4 view = glm::mat4(1.0);

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

        // random generator 
        std::mt19937 generator(seed);
        std::uniform_real_distribution<float> distribution(-w + 0.6, +w - 0.6);

        for (int i = 0; i < nBoids; i++) {
            boidPositions.push_back(glm::vec3(
                distribution(generator),
                distribution(generator),
                distribution(generator)
            ));
        }

        while (!glfwWindowShouldClose(window)) {
            float currentFrame = static_cast<float>(glfwGetTime()/2);
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // set uniforms
            shader.use();
            shader.uniform("projection", projection);
            shader.uniform("view", view);

            // render phase
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // draw grid
            glDepthMask(GL_FALSE);
            glm::mat4 gridModel = glm::rotate(model, glm::radians(boxAngle), glm::vec3(0, 1, 0));
            shader.uniform("model", gridModel);
            shader.uniform("color", 0.8f, 0.8f, 0.85f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            cube->draw();
            shader.uniform("model", gridModel);
            shader.uniform("color", 0.f, 0.f, 0.f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            grid->draw();
            glDepthMask(GL_TRUE);

            for (glm::vec3 position: boidPositions) {
                glm::mat4 rotated, tmp_model = glm::rotate(glm::translate(model, position), glm::radians(globalAngle), glm::vec3(0, 1, 0));

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
