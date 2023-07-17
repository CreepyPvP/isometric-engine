#ifndef MODEL_HPP
#define MODEL_HPP

#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "engine/mesh.hpp"
#include <string>
#include <vector>
class Model {
public:
  Model(std::string path);

  ~Model();

  std::vector<Mesh> meshes;
private:
  void process_node(aiNode* node, const aiScene* scene);
  Mesh process_mesh(aiMesh* mesh, const aiScene* scene);

};

#endif
