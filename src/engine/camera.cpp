#include <engine/camera.hpp>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
    : position(position), direction(direction), up(up) {}

Camera::~Camera() {}

glm::mat4 Camera::get_view_matrix() { 
  return glm::lookAt(position, position + direction, up);
}

glm::mat4 Camera::get_projection_matrix() {
  return glm::ortho(-1.0, 1.0, -1.0, 1.0);
}
