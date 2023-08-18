#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <cassert>

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// shaders
const char *vertexShaderSource = R"glsl(
    #version 330 core

    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    out vec3 Color;

    void main(){
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        Color = aColor;
    }
)glsl";

const char *fragmentShaderSource = R"glsl(
    #version 330 core

    in vec3 Color;

    out vec4 FragColor;

    void main(){
        FragColor = vec4(Color, 1.0);
    }
)glsl";

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
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    // arrow draw
    float vertices[] = {
        // indices                  // colors
        0.2f, 0.0f, 0.0f,           0.f, 0.f, 0.f,
        -0.2f, 0.0f, 0.05f,          0.f, 0.0f, 0.f,
        -0.3f, 0.1732f, 0.0f,      0.f, 0.0f, 0.f,
        -0.3f, -0.1732f, 0.0f,     0.f, 0.f, 0.f,
        -0.2f, 0.0f, -0.05f,         0.f, 0.f, 0.f,
    };
    unsigned int indices[] = {
        0, 2, 1,
        0, 1, 3,
        1, 2, 4,
        1, 4, 3,
        4, 2, 0,
        0, 3, 4,
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // create shader program
    unsigned int vertexShader, fragmentShader, shaderProgram;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glUseProgram(shaderProgram);

    // perspective matrices
    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(0, 0, -1));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::mat4(1.0);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime()/2);
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // set uniforms
        glm::mat4 rotated = glm::rotate(model, currentFrame, glm::vec3(0, 1, 0));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &rotated[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);

        // render phase
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6*3, GL_UNSIGNED_INT, 0);

        // process screen
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw terminate
    glfwTerminate();

    return 0;
}
