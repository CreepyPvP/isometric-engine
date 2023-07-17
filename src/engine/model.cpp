#include "assimp/material.h"
#include "assimp/mesh.h"
#include "engine/mesh.hpp"
#include "engine/texture.hpp"
#include <engine/model.hpp>
#include <stdexcept>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

Model::Model(std::string path) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    throw std::runtime_error("failed to load import asset: " + path);
  }
  process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode* node, const aiScene* scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(process_mesh(mesh, scene));
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    process_node(node->mChildren[i], scene);
  }
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  bool contains_uv = mesh->mTextureCoords[0];
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    vertex.x = mesh->mVertices[i].x;
    vertex.y = mesh->mVertices[i].y;
    vertex.z = mesh->mVertices[i].z;
    if (contains_uv) {
      vertex.uv_x = mesh->mTextureCoords[0][i].x;
      vertex.uv_y = mesh->mTextureCoords[0][i].y;
    } else {
      vertex.uv_x = 0;
      vertex.uv_y = 0;
    }

    vertices.push_back(vertex);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
  }

  return Mesh(vertices, indices);
}

Model::~Model() {

}
