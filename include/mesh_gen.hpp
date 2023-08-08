#pragma once

#define byte unsigned char

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

Mesh generateVoxelMesh(int sizeX, int sizeY, int sizeZ, byte* data, int textureWidth, int textureHeight);
