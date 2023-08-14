#pragma once

#include <cstdint>
#include <vector>
#include <glm/ext/matrix_float4x4.hpp>

#include <mesh_gen.hpp>
#include <sparse_set.hpp>

typedef uint16_t Entity;


struct Transform {
    glm::mat4 mat;
};

struct World {
    SparseSet<Mesh> meshes;
    SparseSet<Transform> transforms;

    std::vector<Entity> freeEntities;

    Entity spawn();
    void despawn(Entity);
};

World createWorld(uint16 capacity);
