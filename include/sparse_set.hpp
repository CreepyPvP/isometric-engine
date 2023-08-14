#pragma once

#include <cstdlib>
#include <assert.h>
#include <types.hpp>

#define EMPTY_SLOT 65535

template <typename T>
struct View;

template <typename T>
struct SparseSet {
    uint16 capacity;
    // current value count
    uint16 n;
    // highest possible id
    uint16 valueCount;
    
    T* valueStore;
    uint16* entityToIndex;
    uint16* indexToEntity;

    void insert(uint16, T);
    void remove(uint16);
    bool contains(uint16);
    T* get(uint16);
    View<T> list();
};

template <typename T>
struct View {
    int index;
    uint16 entity;
    SparseSet<T>* set;
    T* current;

    bool next();
};

template <typename T> SparseSet<T> createSparseSet(uint16 capacity, uint16 valueCount);


// Impl

template <typename T>
SparseSet<T> createSparseSet(uint16 capacity, uint16 valueCount) {
    SparseSet<T> set;
    set.capacity = capacity;
    set.valueCount = valueCount;
    set.n = 0;
    set.valueStore = (T*) malloc(sizeof(T) * capacity);
    set.entityToIndex = (uint16*) malloc(sizeof(uint16) * valueCount);
    set.indexToEntity = (uint16*) malloc(sizeof(uint16) * capacity);

    for (int i = 0; i < valueCount; ++i) {
        set.entityToIndex[i] = EMPTY_SLOT;
    }

    return set;
}

template <typename T>
void SparseSet<T>::insert(uint16 entity, T component) {
    assert(n < capacity && entityToIndex[entity] == EMPTY_SLOT);
    valueStore[n] = component;
    entityToIndex[entity] = n;
    indexToEntity[n] = entity;
    ++n;
}

template <typename T>
void SparseSet<T>::remove(uint16 entity) {
    uint16 index = entityToIndex[entity];
    assert(index != EMPTY_SLOT);
    entityToIndex[entity] = EMPTY_SLOT;
    --n;
    if (n == 0) {
        return;
    }
    
    uint16 oldLastEntity = indexToEntity[n];
    uint16 oldLastIndex = entityToIndex[oldLastEntity];
    valueStore[index] = valueStore[oldLastIndex];
    indexToEntity[index] = oldLastEntity;
    entityToIndex[oldLastEntity] = index;
}

template <typename T>
bool SparseSet<T>::contains(uint16 entity) {
    assert(entity < valueCount);
    return entityToIndex[entity] != EMPTY_SLOT;
}

template <typename T>
T* SparseSet<T>::get(uint16 entity) {
    assert(contains(entity));
    return valueStore + entityToIndex[entity];
}

template <typename T>
View<T> SparseSet<T>::list() {
    View<T> view;
    view.index = -1;
    view.set = this;
    return view;
}

template <typename T>
bool View<T>::next() {
    ++index;
    bool validElement = index < set->n;
    if (!validElement) {
        return false;
    }
    current = set->valueStore + index;
    entity = set->indexToEntity[index];

    return true;
}
