#pragma once
#ifndef MESH_H
#define MESH_H

#include "mana/core/memoryallocator.h"
//
#include <cstorage/cstorage.h>

#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/utilities/texture.h"

enum VertexType {
  VERTEXSPRITE,
  VERTEXQUAD,
  VERTEXMODEL,
  VERTEXDUALCONTOURING
};

struct VertexSprite {
  vec3 position;
  vec2 tex_coord;
};

struct VertexQuad {
  vec3 position;
};

struct VertexModel {
  vec3 position;
  vec3 normal;
  vec2 tex_coord;
  ivec3 joints_ids;
  vec3 weights;
};

struct VertexDualContouring {
  vec3 position;
  vec3 normal;
};

struct Mesh {
  struct Vector* vertices;
  struct Vector* indices;
  struct Vector* textures;
};

static inline void mesh_sprite_init(struct Mesh* mesh);
static inline void mesh_sprite_assign_vertex(struct Vector* vector, float x, float y, float z, float u, float v);
static inline VkVertexInputBindingDescription mesh_sprite_get_binding_description();
static inline void mesh_sprite_get_attribute_descriptions(VkVertexInputAttributeDescription* attribute_descriptions);
static inline void mesh_quad_init(struct Mesh* mesh);
static inline void mesh_quad_assign_vertex(struct Vector* vector, float x, float y, float z);
static inline VkVertexInputBindingDescription mesh_quad_get_binding_description();
static inline void mesh_quad_get_attribute_descriptions(VkVertexInputAttributeDescription* attribute_descriptions);
static inline void mesh_model_init(struct Mesh* mesh);
static inline void mesh_model_assign_vertex(struct Vector* vector, float x, float y, float z, float r, float g, float b, float u, float v, int joint_id_x, int joint_id_y, int joint_id_z, float weight_x, float weight_y, float weight_z);
static inline VkVertexInputBindingDescription mesh_model_get_binding_description();
static inline void mesh_model_get_attribute_descriptions(VkVertexInputAttributeDescription* attribute_descriptions);
static inline void mesh_dual_contouring_init(struct Mesh* mesh);
static inline void mesh_dual_contouring_assign_vertex(struct Vector* vector, float x, float y, float z, float r, float g, float b);
static inline VkVertexInputBindingDescription mesh_dual_contouring_get_binding_description();
static inline void mesh_dual_contouring_get_attribute_descriptions(VkVertexInputAttributeDescription* attribute_descriptions);
static inline void mesh_delete(struct Mesh* mesh);
static inline void mesh_clear(struct Mesh* mesh);
static inline void mesh_assign_indice(struct Vector* vector, uint32_t indice);

/////////////////////////////////////////////////////////////////////////////////////

static inline void mesh_sprite_init(struct Mesh* mesh) {
  mesh->vertices = calloc(1, sizeof(struct Vector));
  vector_init(mesh->vertices, sizeof(struct VertexSprite));

  mesh->indices = calloc(1, sizeof(struct Vector));
  vector_init(mesh->indices, sizeof(uint32_t));
}

static inline void mesh_sprite_assign_vertex(struct Vector* vector, float x, float y, float z, float u, float v) {
  struct VertexSprite vertex = {{0}};
  vertex.position[0] = x;
  vertex.position[1] = y;
  vertex.position[2] = z;

  vertex.tex_coord[0] = u;
  vertex.tex_coord[1] = v;

  vector_push_back(vector, &vertex);
}

static inline VkVertexInputBindingDescription mesh_sprite_get_binding_description() {
  VkVertexInputBindingDescription binding_description = {0};
  binding_description.binding = 0;
  binding_description.stride = sizeof(struct VertexSprite);
  binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return binding_description;
}

static inline void mesh_sprite_get_attribute_descriptions(VkVertexInputAttributeDescription* attribute_descriptions) {
  attribute_descriptions[0].binding = 0;
  attribute_descriptions[0].location = 0;
  attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[0].offset = offsetof(struct VertexSprite, position);

  attribute_descriptions[1].binding = 0;
  attribute_descriptions[1].location = 1;
  attribute_descriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_descriptions[1].offset = offsetof(struct VertexSprite, tex_coord);
}

/////////////////////////////////////////////////////////////////////////////////////

static inline void mesh_quad_init(struct Mesh* mesh) {
  mesh->vertices = calloc(1, sizeof(struct Vector));
  vector_init(mesh->vertices, sizeof(struct VertexQuad));

  mesh->indices = calloc(1, sizeof(struct Vector));
  vector_init(mesh->indices, sizeof(uint32_t));

  mesh_quad_assign_vertex(mesh->vertices, -0.5f, -0.5f, 0.0f);
  mesh_quad_assign_vertex(mesh->vertices, 0.5f, -0.5f, 0.0f);
  mesh_quad_assign_vertex(mesh->vertices, 0.5f, 0.5f, 0.0f);
  mesh_quad_assign_vertex(mesh->vertices, -0.5f, 0.5f, 0.0f);

  mesh_assign_indice(mesh->indices, 0);
  mesh_assign_indice(mesh->indices, 1);
  mesh_assign_indice(mesh->indices, 2);
  mesh_assign_indice(mesh->indices, 2);
  mesh_assign_indice(mesh->indices, 3);
  mesh_assign_indice(mesh->indices, 0);
}

static inline void mesh_quad_assign_vertex(struct Vector* vector, float x, float y, float z) {
  struct VertexSprite vertex = {{0}};
  vertex.position[0] = x;
  vertex.position[1] = y;
  vertex.position[2] = z;

  vector_push_back(vector, &vertex);
}

static inline VkVertexInputBindingDescription mesh_quad_get_binding_description() {
  VkVertexInputBindingDescription binding_description = {0};
  binding_description.binding = 0;
  binding_description.stride = sizeof(struct VertexSprite);
  binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return binding_description;
}

static inline void mesh_quad_get_attribute_descriptions(VkVertexInputAttributeDescription* attribute_descriptions) {
  attribute_descriptions[0].binding = 0;
  attribute_descriptions[0].location = 0;
  attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[0].offset = offsetof(struct VertexQuad, position);
}

/////////////////////////////////////////////////////////////////////////////////////

static inline void mesh_model_init(struct Mesh* mesh) {
  mesh->vertices = calloc(1, sizeof(struct Vector));
  vector_init(mesh->vertices, sizeof(struct VertexModel));

  mesh->indices = calloc(1, sizeof(struct Vector));
  vector_init(mesh->indices, sizeof(uint32_t));
}

static inline void mesh_model_assign_vertex(struct Vector* vector, float x, float y, float z, float r, float g, float b, float u, float v, int joint_id_x, int joint_id_y, int joint_id_z, float weight_x, float weight_y, float weight_z) {
  struct VertexModel vertex = {{0}};
  vertex.position[0] = x;
  vertex.position[1] = y;
  vertex.position[2] = z;

  vertex.normal[0] = r;
  vertex.normal[1] = g;
  vertex.normal[2] = b;

  vertex.tex_coord[0] = u;
  vertex.tex_coord[1] = v;

  vertex.joints_ids[0] = joint_id_x;
  vertex.joints_ids[1] = joint_id_y;
  vertex.joints_ids[2] = joint_id_y;

  vertex.weights[0] = weight_x;
  vertex.weights[1] = weight_y;
  vertex.weights[2] = weight_y;

  vector_push_back(vector, &vertex);
}

static inline VkVertexInputBindingDescription mesh_model_get_binding_description() {
  VkVertexInputBindingDescription binding_description = {0};
  binding_description.binding = 0;
  binding_description.stride = sizeof(struct VertexModel);
  binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return binding_description;
}

static inline void mesh_model_get_attribute_descriptions(VkVertexInputAttributeDescription* attribute_descriptions) {
  attribute_descriptions[0].binding = 0;
  attribute_descriptions[0].location = 0;
  attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[0].offset = offsetof(struct VertexModel, position);

  attribute_descriptions[1].binding = 0;
  attribute_descriptions[1].location = 1;
  attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[1].offset = offsetof(struct VertexModel, normal);

  attribute_descriptions[2].binding = 0;
  attribute_descriptions[2].location = 2;
  attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_descriptions[2].offset = offsetof(struct VertexModel, tex_coord);

  attribute_descriptions[3].binding = 0;
  attribute_descriptions[3].location = 3;
  attribute_descriptions[3].format = VK_FORMAT_R32G32B32_SINT;
  attribute_descriptions[3].offset = offsetof(struct VertexModel, joints_ids);

  attribute_descriptions[4].binding = 0;
  attribute_descriptions[4].location = 4;
  attribute_descriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[4].offset = offsetof(struct VertexModel, weights);
}

/////////////////////////////////////////////////////////////////////////////////////

static inline void mesh_dual_contouring_init(struct Mesh* mesh) {
  mesh->vertices = calloc(1, sizeof(struct Vector));
  vector_init(mesh->vertices, sizeof(struct VertexDualContouring));

  mesh->indices = calloc(1, sizeof(struct Vector));
  vector_init(mesh->indices, sizeof(uint32_t));
}

static inline void mesh_dual_contouring_assign_vertex(struct Vector* vector, float x, float y, float z, float r, float g, float b) {
  struct VertexDualContouring vertex = {{0}};
  vertex.position[0] = x;
  vertex.position[1] = y;
  vertex.position[2] = z;

  vertex.normal[0] = r;
  vertex.normal[1] = g;
  vertex.normal[2] = b;

  vector_push_back(vector, &vertex);
}

static inline VkVertexInputBindingDescription mesh_dual_contouring_get_binding_description() {
  VkVertexInputBindingDescription binding_description = {0};
  binding_description.binding = 0;
  binding_description.stride = sizeof(struct VertexDualContouring);
  binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return binding_description;
}

static inline void mesh_dual_contouring_get_attribute_descriptions(VkVertexInputAttributeDescription* attribute_descriptions) {
  attribute_descriptions[0].binding = 0;
  attribute_descriptions[0].location = 0;
  attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[0].offset = offsetof(struct VertexDualContouring, position);

  attribute_descriptions[1].binding = 0;
  attribute_descriptions[1].location = 1;
  attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[1].offset = offsetof(struct VertexDualContouring, normal);
}

/////////////////////////////////////////////////////////////////////////////////////

static inline void mesh_delete(struct Mesh* mesh) {
  vector_delete(mesh->vertices);
  vector_delete(mesh->indices);
}

static inline void mesh_clear(struct Mesh* mesh) {
  vector_clear(mesh->vertices);
  vector_clear(mesh->indices);
}

static inline void mesh_assign_indice(struct Vector* vector, uint32_t indice) {
  vector_push_back(vector, &indice);
}

#endif  // MESH_H
