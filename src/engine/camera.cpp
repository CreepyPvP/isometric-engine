#include <engine/camera.hpp>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

Camera::Camera() {}

Camera::~Camera() {}

glm::mat4 Camera::get_view_matrix() {
  // return glm::lookAt(position, position + direction, up);
  // return glm::lookAt(glm::vec3(10.0f), glm::vec3(0.0f), glm::vec3(0.0f,
  // 0.0f, 1.0f));
  return glm::lookAt(glm::vec3(100.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                     glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::get_projection_matrix(int screen_width, int screen_height) {
  return glm::ortho((float)-screen_width / 2, (float)screen_width / 2,
                    (float)-screen_height / 2, (float)screen_height / 2, .1f,
                    1000.0f);
}
