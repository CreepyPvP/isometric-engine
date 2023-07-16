#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
class Shader {
public:
  Shader(std::string vertex, std::string fragment);

  ~Shader();

  void use();
private:
  std::string read_file(std::string file);

  unsigned int id;
};

#endif
