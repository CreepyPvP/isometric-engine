#pragma once

#include <cstdint>
#include <vector>
#include <glm/ext/matrix_float4x4.hpp>

#include <mesh_gen.hpp>
#include <sparse_set.hpp>

typedef uint16_t Entity;


struct Transform {
    glm::mat4 mat;
    glm::vec3 pos;
};

struct PointLight {
    unsigned int shadowMap;
    unsigned int framebuffer;
    unsigned int shadowMapDepthBuffer;
    glm::mat4 viewProjectionMat[6];
    unsigned int shadowMapSize;

    glm::vec3 color;

    float attConst;
    float attLinear;
    float attQuad;
};

struct World {
    SparseSet<Mesh> meshes;
    SparseSet<Transform> transforms;
    SparseSet<PointLight> pointLights;

    std::vector<Entity> freeEntities;

    Entity spawn();
    void despawn(Entity);
};

World createWorld(uint16 capacity);
