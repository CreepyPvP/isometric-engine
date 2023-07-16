#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/ext/vector_float3.hpp>
#include <glm/mat4x4.hpp>

class Camera {
public:
  Camera();

  ~Camera();

  glm::mat4 get_view_matrix(); 

  glm::mat4 get_projection_matrix(int screen_width, int screen_height); 

private:
};

#endif
