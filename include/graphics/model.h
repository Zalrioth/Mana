#pragma once
#ifndef MODEL_H_
#define MODEL_H_

#include "core/common.h"
#include "graphics/mesh.h"
#include <assimp/cimport.h> // Plain-C interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h> // Output data structure

struct Model {
    struct aiMatrix4x4 rootMatrix;
    struct Vector* textures_loaded;
    struct Vector* meshes;
    bool gammaCorrection;
};

void init_model(struct Model* model);
void load_model(struct Model* model, char* directory, bool gamma);
void draw_model(struct Model* model);
struct Mesh* process_mode(struct Model* model, struct aiNode* node, struct aiScene* scene);
struct Vector* load_material_textures(struct Model* model, struct aiMaterial* mat, enum aiTextureType type, char* typeName);

#endif // MODEL_H_
