#pragma once
#ifndef MODEL_H_
#define MODEL_H_

#include <assimp/cimport.h>      // Plain-C interface
#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure
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
struct Mesh* model_process_node(struct Model* model, struct aiNode* node, struct aiScene* scene);
struct Vector* model_load_material_textures(struct Model* model, struct aiMaterial* mat, enum aiTextureType type, char* typeName);

#endif  // MODEL_H_
