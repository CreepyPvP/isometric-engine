#pragma once

#include <types.hpp>

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

Mesh generateVoxelMesh(int sizeX, int sizeY, int sizeZ, uchar* data, int textureWidth, int textureHeight);
