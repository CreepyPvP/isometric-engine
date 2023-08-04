#pragma once
#include <glm/ext/matrix_float4x4.hpp>
#include <string>

struct TilemapShader {
    unsigned int id;
    unsigned int uniformModel;
    unsigned int uniformView;
    unsigned int uniformProjection;
};

struct LightingShader {
    unsigned int id;
    unsigned int gPosition;
    unsigned int gNormal;
    unsigned int gAlbedo;
};

TilemapShader createTilemapShader(std::string vertex, std::string fragment);

LightingShader createLightingShader(std::string vertex, std::string fragment);

void useShader(unsigned int id);

void setUniformMat4(unsigned int uniformId, glm::mat4* matrix);

typedef unsigned int Texture;

Texture loadTexture(std::string file);

void bindTexture(Texture texture);
