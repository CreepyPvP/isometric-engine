#pragma once

#include <types.hpp>

#define FEATURE_POS 1
#define FEATURE_UV 1 << 1
#define FEATURE_NORMAL 1 << 2

#define FEATURE_ALL (FEATURE_POS | FEATURE_UV | FEATURE_NORMAL)

struct Vertex {
    float x;
    float y;
    float z;

    float normalX;
    float normalY;
    float normalZ;

    float uvX;
    float uvY;
};

struct Mesh {
    unsigned int vao;
    int indexCount;
};

Mesh generateVoxelMesh(int sizeX, int sizeY, int sizeZ, uchar* data, int textureWidth, int textureHeight, char features);
