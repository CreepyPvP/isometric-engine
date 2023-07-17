#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/ext/vector_float3.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "engine/camera.hpp"
#include "engine/mesh.hpp"
#include "engine/model.hpp"
#include "engine/renderer.hpp"
#include "engine/shader.hpp"
#include "engine/texture.hpp"
#include "engine/window.hpp"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int run();

int main() {
  try {
    return run();
  } catch (std::runtime_error e) {
    std::cout << "Caught error: " << e.what() << std::endl;
    return 1;
  }
}

int run() {
  Window window(SCR_WIDTH, SCR_HEIGHT, "isometric");
  Camera camera;
  Shader shader("../shader/vert.glsl", "../shader/frag.glsl"); 
  Texture texture("../assets/tex.jpg");
  Renderer renderer;
  Model model("../assets/backpack/backpack.obj");

  // Mesh mesh = create_plane();
  // mesh.add_texture(texture);
  // std::vector objects = {mesh}; 

  while (!window.should_close()) {
    window.process_input();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    renderer.render(camera, shader, model.meshes, &window.width, &window.height);

    window.update();
  }

  return 0;
}

