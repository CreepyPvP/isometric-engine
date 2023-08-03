#pragma once
#include <glm/ext/matrix_float4x4.hpp>
#include <string>

struct TilemapShader {
    unsigned int id;
    unsigned int uniformModel;
    unsigned int uniformView;
    unsigned int uniformProjection;
};

TilemapShader createTilemapShader(std::string vertex, std::string fragment);

void setUniformMat4(unsigned int uniformId, glm::mat4* matrix);

void useShader(unsigned int id);

typedef unsigned int Texture;

Texture loadTexture(std::string file);

void bindTexture(Texture texture);
