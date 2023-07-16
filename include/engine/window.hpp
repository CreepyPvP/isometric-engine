#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "GLFW/glfw3.h"
#include <string>

class Window {
public:
  Window(int width, int height, std::string title);

  ~Window();

  bool should_close();

  void update();

  void process_input();

  int width;
  int height;

private:
  GLFWwindow* window;
};

#endif
