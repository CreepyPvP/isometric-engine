#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <engine/window.hpp>

#include <iostream>
#include <stdexcept>


using namespace std;


void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

Window::Window(int width, int height, string title) : width(width), height(height) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  if (window == NULL) {
    throw runtime_error("failed to create window");
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw runtime_error("failed to load required extensions");
  }
}

Window::~Window() {
  glfwTerminate();
}

bool Window::should_close() {
  return glfwWindowShouldClose(window);
}

void Window::update() {
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void Window::process_input() {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}
