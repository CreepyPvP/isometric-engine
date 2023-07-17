#include "engine/texture.hpp"
#include <cstddef>
#include <engine/mesh.hpp>

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
  vertex_count = indices.size();

  glGenVertexArrays(1, &id);
  glGenBuffers(1, &vertex_buffer);
  glGenBuffers(1, &index_buffer);
  glBindVertexArray(id);

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertex_count,
               &vertices[0], GL_STATIC_DRAW);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * vertex_count,
               &indices[0], GL_STATIC_DRAW);

  // position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

  // uv
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, uv_x));

  glBindVertexArray(0);
}

void Mesh::bind() {
  glBindVertexArray(id);

  for (int i = 0; i < textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    textures[i].bind();
  }
}

void Mesh::unbind() {
  glBindVertexArray(0);
}

Mesh::~Mesh() {}

unsigned int Mesh::get_vertex_count() {
  return vertex_count;
}

void Mesh::add_texture(Texture texture) {
  textures.push_back(texture);
}

Mesh create_plane() {
  std::vector<Vertex> vertices = {
    Vertex{ -100, 0, -100, 0, 0 },
    Vertex{ -100, 0,  100, 0, 1 },
    Vertex{  100, 0, -100, 1, 0 },
    Vertex{  100, 0,  100, 1, 1 },
  };

  std::vector<unsigned int> indices = {
    0, 1, 2, 2, 1, 3
  };

  return Mesh(vertices, indices);
}
