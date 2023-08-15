#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>

struct Camera {
    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 up;

    glm::mat4 projection;
    glm::mat4 view;

    float speed;

    void processPlayerInput(GLFWwindow* window, float delta);
    void setScreenSize(float screenX, float screenY);
};

Camera createCamera(float screenX, float screenY);
