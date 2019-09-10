#pragma once
#ifndef MODEL_H_
#define MODEL_H_

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "core/common.h"
#include "graphics/mesh.h"

struct Model {
  struct aiMatrix4x4 root_matrix;
  struct Vector* textures_loaded;
  struct Vector* meshes;
  bool gamma;
};

void model_init(struct Model* model);
int model_load(struct Model* model, char* directory, bool gamma);
void model_draw(struct Model* model);
void model_process_node(struct Model* model, struct aiNode* node, const struct aiScene* scene);
struct Mesh* model_process_mesh(struct Model* model, struct aiMesh* mesh, const struct aiScene* scene);
void model_load_material_textures(struct Vector* vector, struct aiMaterial* mat, enum aiTextureType type, char* path);

#endif  // MODEL_H_
