#include <engine/mesh.hpp>

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Mesh::Mesh(std::vector<Vertex> vertices) {
  vertex_count = vertices.size();

  glGenVertexArrays(1, &id);
  glGenBuffers(3, &pos_buffer);
  glBindVertexArray(id);

  // position
  glBindBuffer(GL_ARRAY_BUFFER, pos_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_count * 3,
               &vertices[0], GL_STATIC_DRAW);

  glBindVertexArray(0);
}

void Mesh::bind() {
  glBindVertexArray(id);

  // attribute 0 : pos
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, pos_buffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Mesh::unbind() {
  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
}

Mesh::~Mesh() {}

unsigned int Mesh::get_vertex_count() {
  return vertex_count;
}

Mesh create_plane() {
  std::vector<Vertex> vertices = {
    Vertex { -100.0f, 0, -100.0f },
    Vertex { -100.0f, 0.0f, 100.0f },
    Vertex { 100.0f, 0.0f, -100.0f },
    Vertex { 100.0f, 0.0f, -100.0f },
    Vertex { -100.0f, 0.0f, 100.0f },
    Vertex { 100.0f, 0.0f, 100.0f },
  };

  return Mesh(vertices);
}
