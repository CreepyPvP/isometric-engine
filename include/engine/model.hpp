#ifndef MODEL_HPP
#define MODEL_HPP

#include "engine/mesh.hpp"
#include <string>
#include <vector>
class Model {
public:
  Model(std::string path);

  ~Model();
private:
  std::vector<Mesh> meshes;
};

#endif
