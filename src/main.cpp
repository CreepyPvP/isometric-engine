// STARTER CODE FROM learnopengl.com

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

#include "engine/window.hpp"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
  try {
    Window window(SCR_WIDTH, SCR_HEIGHT);

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
  } catch (std::runtime_error e) {
    std::cout << "Caught error: " << e.what() << std::endl;
    return 1;
  }
}

