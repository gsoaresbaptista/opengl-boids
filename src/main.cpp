#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "core/shader.hpp"
#include "core/mesh.hpp"
#include "core/primitives.hpp"

#include <vector>
#include <memory>
#include <iostream>

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
    // set opengl context
    assert(glfwInit());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // creating window
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL - Boids", NULL, NULL);
    assert(window);
    glfwMakeContextCurrent(window);

    // load glad
    assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

    // opengl settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glClearColor(0.85f, 0.85f, 0.85f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

    // perspective matrices
    glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, -10));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::mat4(1.0);

    // get circle points
    std::vector<float> circlePoints = Primitives::circle(0.3536, 30);
    Mesh circle(circlePoints);
    std::vector<float> neighborhoodPoints = Primitives::circle(0.3536*5, 100);
    Mesh neighborhood(neighborhoodPoints);

    // algorithm
    bool drawCollisionRegion = false;
    bool drawNeighborhood = false;

    glm::vec3 boidPositions[] = {
        glm::vec3(0, 0, 0),
        glm::vec3(3, 0, -2),
        glm::vec3(-2.5, 3, 0),
    };

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

        for (glm::vec3 position: boidPositions) {
            glm::mat4 rotated, tmp_model = glm::rotate(glm::translate(model, position), currentFrame, glm::vec3(1, 1, 1));

            if (drawCollisionRegion) {
                shader.uniform("color", 0.2f, 0.2f, 0.2f);
                circle.draw();
                rotated = glm::rotate(tmp_model, (float)M_PI/2, glm::vec3(1, 0, 0));
                shader.uniform("model", rotated);
                circle.draw();
                rotated = glm::rotate(tmp_model, (float)M_PI/2, glm::vec3(0, 1, 0));
                shader.uniform("model", rotated);
                circle.draw();
                rotated = glm::rotate(tmp_model, (float)M_PI/2, glm::vec3(0, 0, 1));
                shader.uniform("model", rotated);
                circle.draw();
            }

            if (drawNeighborhood) {
                shader.uniform("color", 0.75f, 0.75f, 0.75f);
                rotated = glm::rotate(tmp_model, (float)M_PI/2, glm::vec3(1, 0, 0));
                shader.uniform("model", rotated);
                neighborhood.draw();
                rotated = glm::rotate(tmp_model, (float)M_PI/2, glm::vec3(0, 1, 0));
                shader.uniform("model", rotated);
                neighborhood.draw();
                rotated = glm::rotate(tmp_model, (float)M_PI/2, glm::vec3(0, 0, 1));
                shader.uniform("model", rotated);
                neighborhood.draw();
            }

            shader.uniform("model", tmp_model);
            shader.uniform("color", 0.f, 0.f, 0.f);
            bird.draw();
        }

        // process screen
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw terminate
    glfwDestroyWindow(window);
    // glfwTerminate();

    return 0;
}
