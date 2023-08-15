#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <camera.hpp>

Camera createCamera(float screenX, float screenY) {
    Camera camera;
    camera.radius = 10.0f;
    camera.setScreenSize(screenX, screenY);
    camera.update(0);

    return camera;
};

void Camera::update(float time) {
    float camX = sin(time) * radius;
    float camZ = cos(time) * radius;
    view = glm::lookAt(glm::vec3(camX, 3.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
}

void Camera::setScreenSize(float screenX, float screenY) {
    projection = glm::perspective(45.0f, screenX / screenY, 1.0f, 150.0f);
}
