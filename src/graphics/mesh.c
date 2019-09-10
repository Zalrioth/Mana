#include "graphics/mesh.h"

void mesh_init(struct Mesh* mesh) {
  mesh->vertices = malloc(sizeof(struct Vector));
  memset(mesh->vertices, 0, sizeof(struct Vector));
  vector_init(mesh->vertices, sizeof(struct Vertex));

  mesh->indices = malloc(sizeof(struct Vector));
  memset(mesh->indices, 0, sizeof(struct Vector));
  vector_init(mesh->indices, sizeof(uint16_t));
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