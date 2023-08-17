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
    camera.speed = 2.7;
    camera.front = glm::vec3(0, 0, -1);
    camera.up = glm::vec3(0, 1, 0);
    camera.yaw = 0;
    camera.pitch = 0;
    camera.setScreenSize(screenX, screenY);

    updateView(&camera);

    return camera;
};


void Camera::processKeyInput(GLFWwindow* window, float delta) {
    glm::vec3 movement = glm::vec3(0.0f);
    bool moved = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        movement.x += 1;
        moved = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        movement.x -= 1;
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        movement.z -= 1;
        moved = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        movement.z += 1;
        moved = true;
    }
    
    if (moved) {
        movement = glm::normalize(movement) * speed * delta;
        pos += movement.x * front;
        pos += movement.z * glm::normalize(glm::cross(front, up));
        updateView(this);
    }
}

void Camera::processMouseInput(float x, float y) {
    yaw += x;
    pitch -= y;

    if (pitch > 89.0) {
        pitch = 89.0;
    } else if (pitch < -89.0) {
        pitch = -89.0;
    }

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);

    updateView(this);
}

inline void Camera::setScreenSize(float screenX, float screenY) {
    projection = glm::perspective(glm::radians(55.0f), screenX / screenY, 1.0f, 150.0f);
}
