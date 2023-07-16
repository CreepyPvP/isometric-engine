#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/ext/vector_float3.hpp>
#include <glm/mat4x4.hpp>

class Camera {
public:
  Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up);

  ~Camera();

  glm::mat4 get_view_matrix(); 

  glm::mat4 get_projection_matrix(); 

private:
  glm::vec3 position;
  glm::vec3 direction;
  glm::vec3 up;
};

#endif
