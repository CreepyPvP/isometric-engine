#include "engine/texture.hpp"
#include <engine/mesh.hpp>

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Mesh::Mesh(std::vector<float> vertices, std::vector<float> uvs) {
  vertex_count = vertices.size() / 3;

  glGenVertexArrays(1, &id);
  glGenBuffers(2, vertex_buffers);
  glBindVertexArray(id);

  // position
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_count * 3,
               &vertices[0], GL_STATIC_DRAW);
  
  // uv
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_count * 2,
               &uvs[0], GL_STATIC_DRAW);

  glBindVertexArray(0);
}

void Mesh::bind() {
  glBindVertexArray(id);

  // attribute 0 : pos
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[0]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // attribute 1 : uv
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[1]);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

  for (int i = 0; i < textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    textures[i].bind();
  }
}

void Mesh::unbind() {
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
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
  std::vector<float> pos = {
    -100.0f, 0, -100.0f ,
    -100.0f, 0.0f, 100.0f,
    100.0f, 0.0f, -100.0f,
    100.0f, 0.0f, -100.0f,
    -100.0f, 0.0f, 100.0f,
    100.0f, 0.0f, 100.0f ,
  };
  std::vector<float> uvs = {
    0, 0,
    0, 1,
    1, 0,
    1, 0,
    0, 1,
    1, 1,
  };

  return Mesh(pos, uvs);
}
