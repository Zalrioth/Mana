#include "mana/graphics/utilities/mesh.h"

void mesh_init(struct Mesh* mesh) {
  mesh->vertices = calloc(1, sizeof(struct Vector));
  vector_init(mesh->vertices, sizeof(struct Vertex));

  mesh->indices = calloc(1, sizeof(struct Vector));
  vector_init(mesh->indices, sizeof(uint32_t));
}

void mesh_delete(struct Mesh* mesh) {
  vector_delete(mesh->vertices);
  vector_delete(mesh->indices);
}

void mesh_assign_vertex(struct Vector* vector, float x, float y, float z, float r, float g, float b, float u, float v) {
  struct Vertex vertex = {{0}};
  vertex.position[0] = x;
  vertex.position[1] = y;
  vertex.position[2] = z;

  vertex.normal[0] = r;
  vertex.normal[1] = g;
  vertex.normal[2] = b;

  vertex.tex_coord[0] = u;
  vertex.tex_coord[1] = v;

  vector_push_back(vector, &vertex);
}

void mesh_assign_vertex_full(struct Vector* vector, float x, float y, float z, float r, float g, float b, float u, float v, float tan_x, float tan_y, float tan_z, float bit_x, float bit_y, float bit_z) {
  struct Vertex vertex = {{0}};
  vertex.position[0] = x;
  vertex.position[1] = y;
  vertex.position[2] = z;

  vertex.normal[0] = r;
  vertex.normal[1] = g;
  vertex.normal[2] = b;

  vertex.tex_coord[0] = u;
  vertex.tex_coord[1] = v;

  vertex.tangent[0] = tan_x;
  vertex.tangent[1] = tan_y;
  vertex.tangent[2] = tan_z;

  vertex.bit_tangent[0] = bit_x;
  vertex.bit_tangent[1] = bit_y;
  vertex.bit_tangent[2] = bit_z;

  vector_push_back(vector, &vertex);
}

void mesh_assign_indice(struct Vector* vector, uint32_t indice) {
  vector_push_back(vector, &indice);
}

VkVertexInputBindingDescription mesh_get_binding_description() {
  VkVertexInputBindingDescription binding_description = {0};
  binding_description.binding = 0;
  binding_description.stride = sizeof(struct Vertex);
  binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return binding_description;
}

void mesh_get_attribute_descriptions(VkVertexInputAttributeDescription* attribute_descriptions) {
  attribute_descriptions[0].binding = 0;
  attribute_descriptions[0].location = 0;
  attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[0].offset = offsetof(struct Vertex, position);

  attribute_descriptions[1].binding = 0;
  attribute_descriptions[1].location = 1;
  attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[1].offset = offsetof(struct Vertex, normal);

  attribute_descriptions[2].binding = 0;
  attribute_descriptions[2].location = 2;
  attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_descriptions[2].offset = offsetof(struct Vertex, tex_coord);

  attribute_descriptions[3].binding = 0;
  attribute_descriptions[3].location = 3;
  attribute_descriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[3].offset = offsetof(struct Vertex, tangent);

  attribute_descriptions[4].binding = 0;
  attribute_descriptions[4].location = 4;
  attribute_descriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[4].offset = offsetof(struct Vertex, bit_tangent);
}
