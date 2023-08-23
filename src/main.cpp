#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "core/shader.hpp"
#include "core/mesh.hpp"
#include "shapes/primitives.hpp"
#include "shapes/visualization.hpp"
#include "camera/orbital_camera.hpp"
#include "utils/imgui.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

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
const glm::vec3 UP(0, 1, 0);
const unsigned int seed = 42;
std::mt19937 generator(seed);

// camera settings
float theta = 0.f;
float phi = 90.f;
float radius = 100.f;
OrbitalCamera camera(radius, theta, phi, glm::vec3(0.f, 0.f, 0.f));

// simulation data
std::vector<glm::vec3> boidPositions;
std::vector<glm::vec3> boidVelocities;

// simulation settings
unsigned int nBoids = 500;
unsigned int grids = 50;
float space = 1.0;
float w = grids * space / 2.0;
const float boidSize = 0.3536;
float maxSpeed = 2.f;
float perceptionRadius = 8*boidSize;
float lastPerceptionRadius = perceptionRadius;
float separationValue = 0.12;
float cohesionValue = 0.12;
float alignmentValue = 0.12;
bool drawCollisionRegion = false;
bool drawNeighborhood = false;
bool drawGrid = true;
bool drawBox = true;
bool running = true;

// imgui settings
unsigned int menuWidth = 260;

void generateBoids() {
    boidPositions = std::vector<glm::vec3>();
    boidVelocities = std::vector<glm::vec3>();

    // random generator
    std::uniform_real_distribution<float> position(-w + 0.6, +w - 0.6);
    std::uniform_real_distribution<float> velocity(-maxSpeed, maxSpeed);

    for (unsigned int i = 0; i < nBoids; i++) {
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

glm::vec3 boidBehavior(unsigned int i, std::vector<glm::vec3> &boidPositions, std::vector<glm::vec3> &boidVelocities) {
    int total = 0;
    glm::vec3 separation = glm::vec3(0);
    glm::vec3 cohesion = glm::vec3(0);
    glm::vec3 alignment = glm::vec3(0);

    for (unsigned int j = 0; j < boidPositions.size(); j++) {
        if (j != i) {
            glm::vec3 distance = boidPositions[j] - boidPositions[i];
            if (glm::length(distance) < perceptionRadius) {
                total++;
                separation += distance;
                cohesion += boidPositions[j];
                alignment += boidVelocities[j];
            }
        }
    }

    if (total > 0) {
        separation = -glm::normalize(separation);
        cohesion /= total;
        cohesion = glm::normalize(cohesion - boidPositions[i]);
        alignment /= total;
        alignment = glm::normalize(alignment);
    }

    return separation * separationValue + cohesion * cohesionValue + alignment * alignmentValue;
}

void updateBoids(std::vector<glm::vec3> &boidPositions, std::vector<glm::vec3> &boidVelocities, float dt) {
    for (unsigned int i = 0; i < boidPositions.size(); i++) {
        glm::vec3 acceleration = boidBehavior(i, boidPositions, boidVelocities);
        boidVelocities[i] += acceleration / (separationValue + cohesionValue + alignmentValue);

        if (glm::length(boidVelocities[i]) > maxSpeed) {
            boidVelocities[i] /= glm::length(boidVelocities[i]);
        }

        // check if boids go out of the cube, if so wrap them to the other side
        boidPositions[i] += boidVelocities[i] * dt;
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

    // load glad
    assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

    // opengl settings
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.85f, 0.85f, 0.85f, 1.0f);
    glViewport(0, 0, windowWidth - menuWidth, windowHeight);

    // setup dear imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // imgui font settings
    io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Regular.ttf", 14.0f);
    ImFont* fontTitle = io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Regular.ttf", 16.0f);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    ImGui_UpdateStyle();
    ImVec2 windowPos(windowWidth - menuWidth, 0);
    ImVec2 windowSize(menuWidth, windowHeight);

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
        std::shared_ptr<Mesh> grid = Visualization::halfCubeGrid(space, grids);
        std::shared_ptr<Mesh> cube = Visualization::halfCube(grids * space);
        std::shared_ptr<Mesh> cubeBorders = Visualization::halfCubeBorders(grids * space);

        // perspective matrices
        glm::mat4 model = glm::mat4(1.0);
        glm::mat4 projection = glm::perspective(glm::radians(40.0f), (float)(windowWidth - menuWidth) / windowHeight, 0.1f, 250.0f);

        // get circle points
        std::shared_ptr<Mesh> circle = Primitives::circle(0.3536, 30);
        std::shared_ptr<Mesh> neighborhood = Primitives::circle(perceptionRadius, 100);

        // generate random boids
        generateBoids();

        while (!glfwWindowShouldClose(window)) {
            // imgui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // imgui window
            ImGui::SetNextWindowPos(windowPos, ImGuiCond_Appearing);
            ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);
            ImGui::Begin("Simulation Settings", NULL, ImGuiWindowFlags_NoMove);
            ImGui::TextWrapped("Welcome to Flocking Simulation! Use these controls to adjust visualization settings and simulation parameters.");
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::PushFont(fontTitle);
            ImGui::Text("Visualization:");
            ImGui::PopFont();
            ImGui::TextWrapped("Setting what is being rendered.");
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::SliderFloat("Altitude", &phi, 0.0f, 90.0f, "%.4f");
            ImGui::SliderFloat("Azimuth", &theta, 0.0f, 360.0f, "%.4f");
            ImGui::SliderFloat("Radius", &radius, 0.1f, 200.0f, "%.2f");
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Columns(2, "drawingFunctions", false);
            ImGui::Checkbox("Collision Region", &drawCollisionRegion);
            ImGui::Checkbox("Percep. Region", &drawNeighborhood); ImGui::NextColumn();
            ImGui::Checkbox("Cube Background", &drawBox);
            ImGui::Checkbox("Cube Grid", &drawGrid);
            ImGui::Columns(1);
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::PushFont(fontTitle);
            ImGui::Text("Behavior:");
            ImGui::PopFont();
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::TextWrapped("Flocking behaviors constant values.");
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::SliderFloat("Alignment", &alignmentValue, 0.0f, 1.0f, "%.4f");
            ImGui::SliderFloat("Cohesion", &cohesionValue, 0.0f, 1.0f, "%.4f");
            ImGui::SliderFloat("Separation", &separationValue, 0.0f, 1.0f, "%.4f");
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::PushFont(fontTitle);
            ImGui::Text("Boid:");
            ImGui::PopFont();
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::TextWrapped("Settings for an individual boid.");
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::SliderFloat("Perception", &perceptionRadius, boidSize, 20*boidSize, "%.4f");
            ImGui::SliderFloat("Max. Speed", &maxSpeed, 0, 100, "%.2f");
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::PushFont(fontTitle);
            ImGui::Text("Simulation:");
            ImGui::PopFont();
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Columns(3, "simulationStatus", false);
            if (ImGui::Button("Stop", ImVec2(75, 20))) {
                running = false;
            }
            ImGui::NextColumn();
            if (ImGui::Button("Play", ImVec2(75, 20))) {
                running = true;
            }
            ImGui::NextColumn();
            if (ImGui::Button("Restart", ImVec2(75, 20))) {
                generator = std::mt19937(seed);
                generateBoids();

            }
            ImGui::End();

            // update neighborhood size
            if (lastPerceptionRadius != perceptionRadius) {
                lastPerceptionRadius = perceptionRadius;
                neighborhood = Primitives::circle(perceptionRadius, 100);
            }

            // update camera angles
            camera.setRadius(radius);
            camera.setTheta(theta);
            camera.setPhi(phi);

            // simulation
            float currentFrame = static_cast<float>(glfwGetTime()/2);
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            if (running) {
                updateBoids(boidPositions, boidVelocities, deltaTime);
            }
            glm::mat4 view = camera.getViewMatrix();

            // set uniforms
            shader.use();
            shader.uniform("projection", projection);
            shader.uniform("view", view);

            // render phase
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // draw grid
            glm::mat4 cubeModel = glm::mat4(1.0);
            float rotatedTheta = glm::radians(int(camera.getTheta() / 90)* 90.0f);
            cubeModel = glm::rotate(cubeModel, rotatedTheta, UP);

            glDepthMask(GL_FALSE);
            if (drawBox) {
                shader.uniform("model", cubeModel);
                shader.uniform("color", 0.8f, 0.8f, 0.85f);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                cube->draw();
                shader.uniform("color", 0.f, 0.f, 0.f);
                cubeBorders->draw();
            }
            if (drawGrid) {
                shader.uniform("model", cubeModel);
                shader.uniform("color", 0.f, 0.f, 0.f);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                grid->draw();
            }
            glDepthMask(GL_TRUE);

            for (unsigned int i = 0; i < boidPositions.size(); i++) {
                glm::mat4 rotated, tmp_model = glm::translate(model, boidPositions[i]);

                if (drawCollisionRegion) {
                    shader.uniform("color", 0.75f, 0.50f, 0.50f);
                    circle->draw();
                    rotated = glm::rotate(tmp_model, (float)M_PI/2, glm::vec3(1, 0, 0));
                    shader.uniform("model", rotated);
                    circle->draw();
                    rotated = glm::rotate(tmp_model, (float)M_PI/2, UP);
                    shader.uniform("model", rotated);
                    circle->draw();
                    rotated = glm::rotate(tmp_model, (float)M_PI/2, glm::vec3(0, 0, 1));
                    shader.uniform("model", rotated);
                    circle->draw();
                }

                if (drawNeighborhood) {
                    shader.uniform("color", 0.5f, 0.5f, 0.75f);
                    rotated = glm::rotate(tmp_model, (float)M_PI/2, glm::vec3(1, 0, 0));
                    shader.uniform("model", rotated);
                    neighborhood->draw();
                    rotated = glm::rotate(tmp_model, (float)M_PI/2, UP);
                    shader.uniform("model", rotated);
                    neighborhood->draw();
                    rotated = glm::rotate(tmp_model, (float)M_PI/2, glm::vec3(0, 0, 1));
                    shader.uniform("model", rotated);
                    neighborhood->draw();
                }

                // fix bird rotation
                glm::vec3 v = boidVelocities[i];
                tmp_model = glm::rotate(tmp_model, -atan2f(v.z, v.x), UP);
                tmp_model = glm::rotate(tmp_model, atan2f(v.y, sqrtf(v.x*v.x+v.z*v.z)), glm::vec3(0, 0, 1));
                shader.uniform("model", tmp_model);
                shader.uniform("color", 0.f, 0.f, 0.f);
                bird.draw();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // process screen
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    // glfw terminate
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
