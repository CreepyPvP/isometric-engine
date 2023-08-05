#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <chunk.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define tileSize 100
#define byte unsigned char

Chunk createChunk(int capactiy) {
    Chunk chunk;
    chunk.vao = -1;
    chunk.tilemapCount = 0;
    chunk.tilemapCapacity = capactiy;
    chunk.tilemaps = (Tilemap*) malloc(capactiy * sizeof(Tilemap));
    return chunk;
}

void Chunk::dispose() {
    free(tilemaps);
}

void Chunk::createTilemap(int width, int height, byte* tileData, glm::vec3 position, glm::vec3 rotation) {
    assert(tilemapCount < tilemapCapacity);
    assert(vao == -1);

    Tilemap tilemap;
    tilemap.tileData = tileData;
    tilemap.width = width;
    tilemap.height = height;

    auto transform = glm::mat4(1.0f);
    transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::translate(transform, position);
    transform = glm::scale(transform, glm::vec3(tileSize * width, tileSize * height, 1.0f));
    tilemap.transform = transform;

    tilemaps[tilemapCount] = tilemap;

    ++tilemapCount;
}

void Chunk::build() {
    float vertices[] = {
        0, 0, 0,    // position 1
        0, 0, 1,    // normal 1

        0, 1, 0,    // position 2
        0, 0, 1,    // normal 2

        1, 0, 0,    // position 3
        0, 0, 1,    // normal 3

        1, 1, 0,    // position 4
        0, 0, 1,    // normal 4
    };

    unsigned int indices[] = {
        0, 1, 2, 
        2, 1, 3
    };

    int tileCount = 0;
    for (byte i = 0; i < tilemapCount; ++i) {
        tileCount += tilemaps[i].width * tilemaps[i].height;
    }

    byte* tileData = (byte*) malloc(sizeof(byte) * tileCount);
    int index = 0;
    for (int tilemap = 0; tilemap < tilemapCount; ++tilemap) {
        size_t size = tilemaps[tilemap].width * tilemaps[tilemap].height;
        memcpy(tileData + index, tilemaps[tilemap].tileData, size);
        index += size;
        tilemaps[tilemap].tileData = nullptr;
    }

    int indexCount = 6;
    int vertexCount = 4;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int buffers[3];
    glGenBuffers(3, buffers);
    unsigned int vertexBuffer = buffers[0];
    unsigned int indexBuffer = buffers[1];
    unsigned int tileDataBuffer = buffers[2];

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexCount * 6, &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexCount, &indices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, tileDataBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(byte) * tileCount, &tileData[0], GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(int) * 6, 0);
    glEnableVertexAttribArray(0);

    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(int) * 6, (void*) (sizeof(int) * 3));
    glEnableVertexAttribArray(1);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(int) * 6, (GLvoid*) (sizeof(int) * 3));

    // tile data
    glEnableVertexAttribArray(2);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, tileDataBuffer);

    glBindVertexArray(0);

    free(tileData);
}

void Chunk::render(TilemapShader* shader) {
    assert(vao != -1);

    glBindVertexArray(vao);

    int offset = 0;
    for (int i = 0; i < tilemapCount; ++i) {
        int width = tilemaps[i].width;
        int height = tilemaps[i].height;

        setUniformMat4(shader->uniformModel, &tilemaps[i].transform);
        setUniform1i(shader->uniformWidth, width);
        setUniform1i(shader->uniformHeight, height);
        setUniform1i(shader->uniformTileDataOffset, offset);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        offset += width * height;
    }
}
