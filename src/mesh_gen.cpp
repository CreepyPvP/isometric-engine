#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <mesh_gen.hpp>

#define internal static

internal unsigned int generateMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int buffers[2];
    glGenBuffers(2, buffers);
    unsigned int vertexBuffer = buffers[0];
    unsigned int indexBuffer = buffers[1];

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, x));
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normalX));
    // uv
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, uvX));

    glBindVertexArray(0);

    return vao;
}

internal int getChunkDataOffset(int x, int y, int z, int sizeX, int sizeZ) {
    return x + z * sizeX + y * sizeX * sizeZ;
}

Mesh generateVoxelMesh(int sizeX, int sizeY, int sizeZ, uchar* data, int textureWidth, int textureHeight) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    uchar* ptr = data;
    for (int y = 0; y < sizeY; ++y) {
        for (int z = 0; z < sizeZ; ++z) {
            for (int x = 0; x < sizeX; ++x) {
                uchar value = *ptr;
                if (value == 255) {
                    ++ptr;
                    continue;
                }

                int textureX = value % textureWidth;
                int textureY = (value - textureX) / textureHeight;

                float tx = textureX;
                float ty = textureY;
                // This is really ugly holy fuck i hope i never have to touch it again
                if (!(y + 1 < sizeY) || data[getChunkDataOffset(x, y + 1, z, sizeX, sizeZ)] == 255)
                {
                    // top side
                    Vertex v1;
                    Vertex v2;
                    Vertex v3;
                    Vertex v4;

                    v1.x = x - 0.5;
                    v1.y = y + 0.5;
                    v1.z = z - 0.5;
                    v1.uvX = tx / textureWidth;
                    v1.uvY = ty / textureHeight;
                    v1.normalX = 0;
                    v1.normalY = 1;
                    v1.normalZ = 0;

                    v2.x = x + 0.5;
                    v2.y = y + 0.5;
                    v2.z = z - 0.5;
                    v2.uvX = (tx + 1) / textureWidth;
                    v2.uvY = ty / textureHeight;
                    v2.normalX = 0;
                    v2.normalY = 1;
                    v2.normalZ = 0;

                    v3.x = x - 0.5;
                    v3.y = y + 0.5;
                    v3.z = z + 0.5;
                    v3.uvX = tx / textureWidth;
                    v3.uvY = (ty + 1) / textureHeight;
                    v3.normalX = 0;
                    v3.normalY = 1;
                    v3.normalZ = 0;

                    v4.x = x + 0.5;
                    v4.y = y + 0.5;
                    v4.z = z + 0.5;
                    v4.uvX = (tx + 1) / textureWidth;
                    v4.uvY = (ty + 1) / textureHeight;
                    v4.normalX = 0;
                    v4.normalY = 1;
                    v4.normalZ = 0;

                    unsigned int vertexOffset = vertices.size();

                    Vertex vert[] = {v1, v2, v3, v4};
                    vertices.insert(vertices.end(), vert, vert + 4);

                    unsigned int index[] = {
                        0 + vertexOffset, 
                        1 + vertexOffset, 
                        2 + vertexOffset, 
                        1 + vertexOffset, 
                        3 + vertexOffset, 
                        2 + vertexOffset
                    };
                    indices.insert(indices.end(), index, index + 6);
                }
                if (!(y - 1 <= 0) || data[getChunkDataOffset(x, y - 1, z, sizeX, sizeZ)] == 255)
                {
                    // bottom side
                    Vertex v1;
                    Vertex v2;
                    Vertex v3;
                    Vertex v4;

                    v1.x = x + 0.5;
                    v1.y = y - 0.5;
                    v1.z = z - 0.5;
                    v1.uvX = (tx + 1) / textureWidth;
                    v1.uvY = ty / textureHeight;
                    v1.normalX = 0;
                    v1.normalY = -1;
                    v1.normalZ = 0;

                    v2.x = x - 0.5;
                    v2.y = y - 0.5;
                    v2.z = z - 0.5;
                    v2.uvX = tx / textureWidth;
                    v2.uvY = ty / textureHeight;
                    v2.normalX = 0;
                    v2.normalY = -1;
                    v2.normalZ = 0;

                    v3.x = x + 0.5;
                    v3.y = y - 0.5;
                    v3.z = z + 0.5;
                    v3.uvX = (tx + 1) / textureWidth;
                    v3.uvY = (ty + 1) / textureHeight;
                    v3.normalX = 0;
                    v3.normalY = -1;
                    v3.normalZ = 0;

                    v4.x = x - 0.5;
                    v4.y = y - 0.5;
                    v4.z = z + 0.5;
                    v4.uvX = tx / textureWidth;
                    v4.uvY = (ty + 1) / textureHeight;
                    v4.normalX = 0;
                    v4.normalY = -1;
                    v4.normalZ = 0;

                    unsigned int vertexOffset = vertices.size();

                    Vertex vert[] = {v1, v2, v3, v4};
                    vertices.insert(vertices.end(), vert, vert + 4);

                    unsigned int index[] = {
                        0 + vertexOffset, 
                        1 + vertexOffset, 
                        2 + vertexOffset, 
                        1 + vertexOffset, 
                        3 + vertexOffset, 
                        2 + vertexOffset
                    };
                    indices.insert(indices.end(), index, index + 6);
                }
                if (!(x - 1 <= 0) || data[getChunkDataOffset(x - 1, y, z, sizeX, sizeZ)] == 255)
                {
                    // left side
                    Vertex v1;
                    Vertex v2;
                    Vertex v3;
                    Vertex v4;

                    v1.x = x - 0.5;
                    v1.y = y + 0.5;
                    v1.z = z - 0.5;
                    v1.uvX = tx / textureWidth;
                    v1.uvY = ty / textureHeight;
                    v1.normalX = -1;
                    v1.normalY = 0;
                    v1.normalZ = 0;

                    v2.x = x - 0.5;
                    v2.y = y + 0.5;
                    v2.z = z + 0.5;
                    v2.uvX = (tx + 1) / textureWidth;
                    v2.uvY = ty / textureHeight;
                    v2.normalX = -1;
                    v2.normalY = 0;
                    v2.normalZ = 0;

                    v3.x = x - 0.5;
                    v3.y = y - 0.5;
                    v3.z = z - 0.5;
                    v3.uvX = tx / textureWidth;
                    v3.uvY = (ty + 1) / textureHeight;
                    v3.normalX = -1;
                    v3.normalY = 0;
                    v3.normalZ = 0;

                    v4.x = x - 0.5;
                    v4.y = y - 0.5;
                    v4.z = z + 0.5;
                    v4.uvX = (tx + 1) / textureWidth;
                    v4.uvY = (ty + 1) / textureHeight;
                    v4.normalX = -1;
                    v4.normalY = 0;
                    v4.normalZ = 0;

                    unsigned int vertexOffset = vertices.size();

                    Vertex vert[] = {v1, v2, v3, v4};
                    vertices.insert(vertices.end(), vert, vert + 4);

                    unsigned int index[] = {
                        0 + vertexOffset, 
                        1 + vertexOffset, 
                        2 + vertexOffset, 
                        1 + vertexOffset, 
                        3 + vertexOffset, 
                        2 + vertexOffset
                    };
                    indices.insert(indices.end(), index, index + 6);
                }
                if (!(x + 1 < sizeX) || data[getChunkDataOffset(x + 1, y, z, sizeX, sizeZ)] == 255)
                {
                    // right side
                    Vertex v1;
                    Vertex v2;
                    Vertex v3;
                    Vertex v4;

                    v1.x = x + 0.5;
                    v1.y = y + 0.5;
                    v1.z = z + 0.5;
                    v1.uvX = tx / textureWidth;
                    v1.uvY = ty / textureHeight;
                    v1.normalX = 1;
                    v1.normalY = 0;
                    v1.normalZ = 0;

                    v2.x = x + 0.5;
                    v2.y = y + 0.5;
                    v2.z = z - 0.5;
                    v2.uvX = (tx + 1) / textureWidth;
                    v2.uvY = ty / textureHeight;
                    v2.normalX = 1;
                    v2.normalY = 0;
                    v2.normalZ = 0;

                    v3.x = x + 0.5;
                    v3.y = y - 0.5;
                    v3.z = z + 0.5;
                    v3.uvX = tx / textureWidth;
                    v3.uvY = (ty + 1) / textureHeight;
                    v3.normalX = 1;
                    v3.normalY = 0;
                    v3.normalZ = 0;

                    v4.x = x + 0.5;
                    v4.y = y - 0.5;
                    v4.z = z - 0.5;
                    v4.uvX = (tx + 1) / textureWidth;
                    v4.uvY = (ty + 1) / textureHeight;
                    v4.normalX = 1;
                    v4.normalY = 0;
                    v4.normalZ = 0;

                    unsigned int vertexOffset = vertices.size();

                    Vertex vert[] = {v1, v2, v3, v4};
                    vertices.insert(vertices.end(), vert, vert + 4);

                    unsigned int index[] = {
                        0 + vertexOffset, 
                        1 + vertexOffset, 
                        2 + vertexOffset, 
                        1 + vertexOffset, 
                        3 + vertexOffset, 
                        2 + vertexOffset
                    };
                    indices.insert(indices.end(), index, index + 6);
                }
                if (!(z + 1 < sizeZ) || data[getChunkDataOffset(x, y, z + 1, sizeX, sizeZ)] == 255)
                {
                    // front side
                    Vertex v1;
                    Vertex v2;
                    Vertex v3;
                    Vertex v4;

                    v1.x = x - 0.5;
                    v1.y = y + 0.5;
                    v1.z = z + 0.5;
                    v1.uvX = tx / textureWidth;
                    v1.uvY = ty / textureHeight;
                    v1.normalX = 0;
                    v1.normalY = 0;
                    v1.normalZ = 1;

                    v2.x = x + 0.5;
                    v2.y = y + 0.5;
                    v2.z = z + 0.5;
                    v2.uvX = (tx + 1) / textureWidth;
                    v2.uvY = ty / textureHeight;
                    v2.normalX = 0;
                    v2.normalY = 0;
                    v2.normalZ = 1;

                    v3.x = x - 0.5;
                    v3.y = y - 0.5;
                    v3.z = z + 0.5;
                    v3.uvX = tx / textureWidth;
                    v3.uvY = (ty + 1) / textureHeight;
                    v3.normalX = 0;
                    v3.normalY = 0;
                    v3.normalZ = 1;

                    v4.x = x + 0.5;
                    v4.y = y - 0.5;
                    v4.z = z + 0.5;
                    v4.uvX = (tx + 1) / textureWidth;
                    v4.uvY = (ty + 1) / textureHeight;
                    v4.normalX = 0;
                    v4.normalY = 0;
                    v4.normalZ = 1;

                    unsigned int vertexOffset = vertices.size();

                    Vertex vert[] = {v1, v2, v3, v4};
                    vertices.insert(vertices.end(), vert, vert + 4);

                    unsigned int index[] = {
                        0 + vertexOffset, 
                        1 + vertexOffset, 
                        2 + vertexOffset, 
                        1 + vertexOffset, 
                        3 + vertexOffset, 
                        2 + vertexOffset
                    };
                    indices.insert(indices.end(), index, index + 6);
                }
                if (!(z - 1 >= 0) || data[getChunkDataOffset(x, y, z - 1, sizeX, sizeZ)] == 255)
                {
                    // back side
                    Vertex v1;
                    Vertex v2;
                    Vertex v3;
                    Vertex v4;

                    v1.x = x + 0.5;
                    v1.y = y + 0.5;
                    v1.z = z - 0.5;
                    v1.uvX = tx / textureWidth;
                    v1.uvY = ty / textureHeight;
                    v1.normalX = 0;
                    v1.normalY = 0;
                    v1.normalZ = -1;

                    v2.x = x - 0.5;
                    v2.y = y + 0.5;
                    v2.z = z - 0.5;
                    v2.uvX = (tx + 1) / textureWidth;
                    v2.uvY = ty / textureHeight;
                    v2.normalX = 0;
                    v2.normalY = 0;
                    v2.normalZ = -1;

                    v3.x = x + 0.5;
                    v3.y = y - 0.5;
                    v3.z = z - 0.5;
                    v3.uvX = tx / textureWidth;
                    v3.uvY = (ty + 1) / textureHeight;
                    v3.normalX = 0;
                    v3.normalY = 0;
                    v3.normalZ = -1;

                    v4.x = x - 0.5;
                    v4.y = y - 0.5;
                    v4.z = z - 0.5;
                    v4.uvX = (tx + 1) / textureWidth;
                    v4.uvY = (ty + 1) / textureHeight;
                    v4.normalX = 0;
                    v4.normalY = 0;
                    v4.normalZ = -1;

                    unsigned int vertexOffset = vertices.size();

                    Vertex vert[] = {v1, v2, v3, v4};
                    vertices.insert(vertices.end(), vert, vert + 4);

                    unsigned int index[] = {
                        0 + vertexOffset, 
                        1 + vertexOffset, 
                        2 + vertexOffset, 
                        1 + vertexOffset, 
                        3 + vertexOffset, 
                        2 + vertexOffset
                    };
                    indices.insert(indices.end(), index, index + 6);
                }
                ++ptr;
            }
        }
    }

    Mesh mesh;
    mesh.vao = generateMesh(vertices, indices);
    mesh.indexCount = indices.size();

    return mesh;
}
