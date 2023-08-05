#pragma once

#include <glm/mat4x4.hpp>
#include <opengl.hpp>

struct Tilemap {
    glm::mat4 transform;
    int width;
    int height;
    unsigned char* tileData;
};

struct Chunk {
    unsigned int vao;

    Tilemap* tilemaps;
    int tilemapCount;
    int tilemapCapacity;

    // Important: tileData has to be valid, until .build() is called
    void createTilemap(int width, int height, unsigned char* tileData, glm::vec3 position, glm::vec3 rotation);
    void dispose();
    void build();
    void render(TilemapShader* shader);
};

Chunk createChunk(int capactiy); 
