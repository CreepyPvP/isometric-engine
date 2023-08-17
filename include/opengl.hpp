#pragma once
#include <glm/ext/matrix_float4x4.hpp>
#include <string>

struct TilemapShader {
    unsigned int id;

    unsigned int uModel;
    unsigned int uView;
    unsigned int uProjection;
    unsigned int uWidth;
    unsigned int uHeight;
    unsigned int uTileDataOffset;
};

struct LightingShader {
    unsigned int id;
    unsigned int gPosition;
    unsigned int gNormal;
    unsigned int gAlbedo;
    unsigned int shadowMap;
    unsigned int cubemapShadow;
    unsigned int uLightSpace;

    unsigned int uCameraPos;

    unsigned int uLightPos;
    unsigned int uLightColor;
};

struct ShadowShader {
    unsigned int id;
    unsigned int uLightSpace;
    unsigned int uModel;
    unsigned int uLightPos;
};

struct ObjectShader {
    unsigned int id;
    unsigned int uModel;
    unsigned int uView;
    unsigned int uProjection;
    unsigned int texture;
};

TilemapShader createTilemapShader(std::string vertex, std::string fragment);

LightingShader createLightingShader(std::string vertex, std::string fragment);

ShadowShader createShadowShader(std::string vertex, std::string fragment);

ObjectShader createObjectShader(std::string vertex, std::string fragment);

void useShader(unsigned int id);

void setUniformMat4(unsigned int uniformId, glm::mat4* matrix);

void setUniform1i(unsigned int uniformId, unsigned int value);

void setUniformVec3(unsigned int uniformId, glm::vec3* vec);

void setUniformVec4(unsigned int uniformId, glm::vec4* vec);

typedef unsigned int Texture;

Texture loadTexture(std::string file);

void bindTexture(Texture texture);
