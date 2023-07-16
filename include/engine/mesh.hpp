#ifndef MESH_HPP
#define MESH_HPP

#include "engine/texture.hpp"
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <vector>

class Mesh {
public:
  Mesh(std::vector<float> vertices, std::vector<float> uvs);

  ~Mesh();

  void bind();
  void unbind();
  unsigned int get_vertex_count();
  void add_texture(Texture texture);

private:
  unsigned int vertex_count;
  unsigned int id;
  // 0: pos
  // 1: uv
  unsigned int vertex_buffers[2];

  std::vector<Texture> textures;
};


Mesh create_plane(); 

#endif
