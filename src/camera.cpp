#include "glm/geometric.hpp"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <camera.hpp>

inline void updateView(Camera* camera) {
    camera->view = glm::lookAt(camera->pos, camera->pos + camera->front, camera->up);
}

Camera createCamera(float screenX, float screenY) {
    Camera camera;
    camera.pos = glm::vec3(0, 2, 0);
    camera.speed = 0.05;
    camera.front = glm::vec3(0, 0, -1);
    camera.up = glm::vec3(0, 1, 0);
    camera.setScreenSize(screenX, screenY);

    updateView(&camera);

    return camera;
};


void Camera::processPlayerInput(GLFWwindow* window, float time) {
    // TODO: use delta time
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        pos += speed * front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        pos -= speed * front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        pos -= glm::normalize(glm::cross(front, up)) * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        pos += glm::normalize(glm::cross(front, up)) * speed;
    
    updateView(this);
}

inline void Camera::setScreenSize(float screenX, float screenY) {
    projection = glm::perspective(45.0f, screenX / screenY, 1.0f, 150.0f);
}
