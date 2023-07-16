#include <engine/shader.hpp>
#include <fstream>
#include <glm/ext/matrix_float4x4.hpp>
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

  get_all_uniforms();
}

Shader::~Shader() {}

void Shader::use() {
  glUseProgram(id);
}

void Shader::set_projection(glm::mat4& projection) {
  glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, &projection[0][0]);
}

void Shader::set_view(glm::mat4& view) {
  glUniformMatrix4fv(uniform_view, 1, GL_FALSE, &view[0][0]);
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

void Shader::get_all_uniforms() {
  uniform_view = glGetUniformLocation(id, "view");
  uniform_projection = glGetUniformLocation(id, "projection");
}
