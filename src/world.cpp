#include <cassert>
#include <numeric>
#include <vector>

#include <sparse_set.hpp>
#include <world.hpp>
#include <mesh_gen.hpp>


World createWorld(uint16_t capacity) {
    World world;
    world.freeEntities = std::vector<Entity>(capacity);
    std::iota(world.freeEntities.begin(), world.freeEntities.end(), 0);

    world.meshes = createSparseSet<Mesh>(100, capacity);
    world.transforms = createSparseSet<Transform>(100, capacity);

    return world;
}

Entity World::spawn() {
    assert(freeEntities.size() >= 1);
    Entity entity = freeEntities.back();
    freeEntities.pop_back();
    return entity;
}

void World::despawn(Entity entity) {
    meshes.remove(entity);
    transforms.remove(entity);

    freeEntities.push_back(entity);
}
