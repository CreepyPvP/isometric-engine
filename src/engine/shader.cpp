#include <engine/shader.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Shader::Shader(std::string v_file, std::string f_file) {
  std::string v_raw = read_file(v_file);
  std::string f_raw = read_file(f_file);

  const char* v_code = v_raw.c_str();
  const char* f_code = f_raw.c_str();

  unsigned int vertex, fragment;
  int success;
  char info_log[512];

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &v_code, NULL);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(vertex, 512, NULL, info_log);
    std::cout << "Error compiling vertex shader: " << info_log << std::endl;
    throw std::runtime_error("failed to compile vertex shader");
  }

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &f_code, NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(fragment, 512, NULL, info_log);
    std::cout << "Error compiling fragment shader: " << info_log << std::endl;
    throw std::runtime_error("failed to compile fragment shader");
  }

  id = glCreateProgram();
  glAttachShader(id, vertex);
  glAttachShader(id, fragment);
  glLinkProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(id, 512, NULL, info_log);
    std::cout << "Error linking shader: " << info_log << std::endl;
    throw std::runtime_error("failed to link shader");
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

Shader::~Shader() {}

void Shader::use() {
  glUseProgram(id);
}

std::string Shader::read_file(std::string file) {
  try {
    std::ifstream handle(file);
    handle.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    std::stringstream buffer;
    buffer << handle.rdbuf();
    return buffer.str();
  } catch (...) {
    throw std::runtime_error("failed to read file: " + file);
  }
}
