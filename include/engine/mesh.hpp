#ifndef MESH_HPP
#define MESH_HPP

#include <vector>

struct Vertex {
  float x;
  float y;
  float z;
};

class Mesh {
public:
  Mesh(std::vector<Vertex> vertices);

  ~Mesh();

  void bind();
  void unbind();
  unsigned int get_vertex_count();

private:
  unsigned int vertex_count;
  unsigned int id;
  unsigned int pos_buffer;
};


Mesh create_plane(); 

#endif
