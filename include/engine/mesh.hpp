#ifndef MESH_HPP
#define MESH_HPP

#include "engine/texture.hpp"
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <vector>

struct Vertex {
  float x;
  float y;
  float z;
  float uv_x;
  float uv_y;
};

class Mesh {
public:
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

  ~Mesh();

  void bind();
  void unbind();
  unsigned int get_vertex_count();
  void add_texture(Texture texture);

private:
  unsigned int vertex_count;
  unsigned int id;
  unsigned int vertex_buffer;
  unsigned int index_buffer;

  std::vector<Texture> textures;
};


Mesh create_plane(); 

#endif
