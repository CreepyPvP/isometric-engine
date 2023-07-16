#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/ext/vector_float3.hpp>
#include <iostream>
#include <stdexcept>

#include "engine/camera.hpp"
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
  Camera camera(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    return -1;
  }
  while (!window.should_close()) {
    window.process_input();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    window.update();
  }

  return 0;
}

