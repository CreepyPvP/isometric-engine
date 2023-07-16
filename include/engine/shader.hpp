#ifndef SHADER_HPP
#define SHADER_HPP

#include <glm/ext/matrix_float4x4.hpp>
#include <string>
class Shader {
public:
  Shader(std::string vertex, std::string fragment);

  ~Shader();

  void use();

  void set_projection(glm::mat4& projection);
  void set_view(glm::mat4& projection);
private:
  std::string read_file(std::string file);
  void get_all_uniforms();

  unsigned int id;

  unsigned int uniform_view;
  unsigned int uniform_projection;
};

#endif
