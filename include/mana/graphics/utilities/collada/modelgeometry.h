#pragma once
#ifndef MODEL_GEOMETRY_H
#define MODEL_GEOMETRY_H

#include "mana/core/memoryallocator.h"
//
#include <cglm/cglm.h>
#include <cstorage/cstorage.h>

#include "mana/core/corecommon.h"
#include "mana/core/xmlnode.h"
#include "mana/graphics/entities/model.h"
#include "mana/graphics/utilities/mesh.h"

struct RawVertexModel {
  vec3 position;
  int texture_index;
  int normal_index;
  struct RawVertexModel* duplicate_vertex;
  int index;
  float length;
  struct VertexSkinData* weights_data;
};

static inline void raw_vertex_model_init(struct RawVertexModel* raw_vertex_model, int index, vec3 position, struct VertexSkinData* weights_data) {
  raw_vertex_model->texture_index = -1;
  raw_vertex_model->normal_index = -1;
  raw_vertex_model->index = index;
  raw_vertex_model->weights_data = weights_data;
  glm_vec3_copy(position, raw_vertex_model->position);
  raw_vertex_model->length = glm_vec3_norm(position);
  raw_vertex_model->duplicate_vertex = NULL;
};

static inline bool raw_vertex_model_is_set(struct RawVertexModel* raw_vertex_model) {
  return raw_vertex_model->texture_index != -1 && raw_vertex_model->normal_index != -1;
}

static inline bool raw_vertex_model_has_same_texture_and_normal(struct RawVertexModel* raw_vertex_model, int texture_index_other, int normal_index_other) {
  return texture_index_other == raw_vertex_model->texture_index && normal_index_other == raw_vertex_model->normal_index;
}

struct ModelData {
  struct Vector* vertices;
  struct Vector* tex_coords;
  struct Vector* normals;
  struct Vector* indices;
  struct Vector* joint_ids;
  struct Vector* vertex_weights;
};

static inline void model_data_init(struct ModelData* model_data) {
  model_data->vertices = malloc(sizeof(struct Vector));
  vector_init(model_data->vertices, sizeof(struct RawVertexModel));

  model_data->tex_coords = malloc(sizeof(struct Vector));
  vector_init(model_data->tex_coords, sizeof(vec2));

  model_data->normals = malloc(sizeof(struct Vector));
  vector_init(model_data->normals, sizeof(vec3));

  model_data->indices = malloc(sizeof(struct Vector));
  vector_init(model_data->indices, sizeof(uint32_t));

  model_data->joint_ids = malloc(sizeof(struct Vector));
  vector_init(model_data->joint_ids, sizeof(ivec3));

  model_data->vertex_weights = malloc(sizeof(struct Vector));
  vector_init(model_data->vertex_weights, sizeof(vec3));
}

struct Mesh* geometry_loader_extract_model_data(struct XmlNode* geometry_node, struct Vector* vertex_weights);
void geometry_loader_read_raw_data(struct ModelData* model_data, struct XmlNode* mesh_data, struct Vector* vertex_weights);
void geometry_loader_read_positions(struct ModelData* model_data, struct XmlNode* mesh_data, struct Vector* vertex_weights);
void geometry_loader_read_normals(struct ModelData* model_data, struct XmlNode* mesh_data);
void geometry_loader_read_texture_coordinates(struct ModelData* model_data, struct XmlNode* mesh_data);
void geometry_loader_assemble_vertices(struct ModelData* model_data, struct XmlNode* mesh_data);
struct RawVertexModel* geometry_loader_process_vertex(struct ModelData* model_data, int position_index, int normal_index, int tex_coord_index);
float geometry_loader_convert_data_to_arrays(struct ModelData* model_data, struct Mesh* model_mesh);
struct RawVertexModel* geometry_loader_deal_with_already_processed_vertex(struct ModelData* model_data, struct RawVertexModel* previous_vertex, int new_texture_index, int new_normal_index);
void geometry_loader_remove_unused_vertices(struct ModelData* model_data);

#endif  // MODEL_GEOMETRY_H
