#include "engine/mesh.hpp"
#include <engine/renderer.hpp>
#include <engine/camera.hpp>
#include <engine/shader.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

void Renderer::render(Camera& camera, Shader& shader, std::vector<Mesh>& objects, int* screen_width, int* screen_height) {
  shader.use();

  auto view = camera.get_view_matrix();
  auto projection = camera.get_projection_matrix(*screen_width, *screen_height);
  shader.set_view(view);
  shader.set_projection(projection);

  for (auto obj : objects) {
    obj.bind();
    glDrawElements(GL_TRIANGLES, obj.get_vertex_count(), GL_UNSIGNED_INT, 0);
    obj.unbind();
  }
}
