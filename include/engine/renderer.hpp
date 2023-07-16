#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "engine/mesh.hpp"
#include "engine/shader.hpp"
#include "engine/camera.hpp"
#include <vector>

class Renderer {
public:

  void render(Camera& camera, Shader& shader, std::vector<Mesh>& objects, int* width, int* height);

private:
};

#endif
