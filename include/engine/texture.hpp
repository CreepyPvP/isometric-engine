#ifndef TEXTURE_HPP
#define TEXTURE_HPP


#include <string>


class Texture {
public:
  Texture(std::string file);

  ~Texture();

  void bind();
private:
  unsigned int texture;
};

#endif

