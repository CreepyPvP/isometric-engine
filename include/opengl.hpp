#pragma once
#include <glm/ext/matrix_float4x4.hpp>
#include <string>

struct Shader {
    unsigned int id;
    unsigned int uniformView;
    unsigned int uniformProjection;
};

Shader createShader(std::string vertex, std::string fragment);

void setUniformMat4(Shader* shader, glm::mat4* matrix);

void useShader(Shader* shader);

typedef unsigned int Texture;

Texture loadTexture(std::string file);

void bindTexture(Texture texture);
