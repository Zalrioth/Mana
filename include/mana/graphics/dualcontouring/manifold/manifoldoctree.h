#pragma once
#ifndef MANIFOLD_OCTREE_H
#define MANIFOLD_OCTREE_H

#include "mana/core/memoryallocator.h"
//
#include <cnoise/cnoise.h>
#include <cstorage/cstorage.h>
#include <ubermath/ubermath.h>

#include "mana/graphics/dualcontouring/manifold/manifolddualcontouring.h"
#include "mana/graphics/dualcontouring/manifold/manifoldtables.h"
#include "mana/graphics/dualcontouring/qef.h"
#include "mana/graphics/utilities/mesh.h"

enum NodeType {
  MANIFOLD_NODE_NONE,
  MANIFOLD_NODE_INTERNAL,
  MANIFOLD_NODE_LEAF,
  MANIFOLD_NODE_COLLAPSED
};

struct Vertex {
  struct Vertex* parent;
  int index;
  bool collapsible;
  struct QefSolver qef;
  vec3 normal;
  int surface_index;
  float error;
  int euler;
  int eis[12];
  int in_cell;
  bool face_prop2;
};

static inline void vertex_init(struct Vertex* vertex) {
  vertex->parent = NULL;
  vertex->index = -1;
  vertex->collapsible = true;
  qef_solver_init(&vertex->qef);
  vertex->normal = VEC3_ZERO;
  vertex->surface_index = -1;
  vertex->error = 0.0f;
  vertex->euler = 0;
  memset(vertex->eis, 0, sizeof(int) * 12);
  vertex->face_prop2 = false;
}

//vec3 vertex_position(struct Vertex* vertex) {
//  if (vertex->qef != NULL)
//    return qef_solve(1e-6f, 4, 1e-6f);
//  return VEC3_ZERO;
//}
//
struct ManifoldOctreeNode {
  int index;
  vec3 position;
  int size;
  struct ManifoldOctreeNode* children[8];
  enum NodeType type;
  struct ArrayList* vertices;
  unsigned char corners;
  int child_index;
};

static inline void octree_node_init(struct ManifoldOctreeNode* octree_node, vec3 position, int size, enum NodeType type) {
  octree_node->index = 0;
  octree_node->position = position;
  octree_node->size = size;
  octree_node->type = type;
  //octree_node->children = new OctreeNode[8];
  //octree_node->vertices = new Vertex[0];
}

//static inline float Sphere(vec3 pos) {
//  return vec3_magnitude(pos) - 0.5f;
//}

static inline float Sphere(vec3 pos) {
  //const float radius = (float)64 / 2.0f - 2.0f;
  //vec3 origin = vec3_set((64 - 2.0f) * 0.5f);
  //return -vec3_square_magnitude(vec3_sub(pos, origin)) + radius * radius;
  float STEP = 1.0 / 64.0f;
  struct RidgedFractalNoise noise = {0};
  ridged_fractal_noise_init(&noise);
  noise.octave_count = 4;
  noise.frequency = 1.0;
  noise.lacunarity = 2.2324f;
  noise.step = STEP;
  return ridged_fractal_noise_eval_3d_single(&noise, pos.x, pos.y, pos.z);
}

static inline vec3 GetIntersection(vec3 p1, vec3 p2, float d1, float d2) {
  return vec3_add(p1, vec3_divs(vec3_scale(vec3_sub(p2, p1), -d1), d2 - d1));
}

// TODO: Calculating normals can be much faster in a special noise function either by a single 8x1 array for SIMD with each position unique in 3D space
// OR calculate
static inline vec3 GetNormal(vec3 v) {
  float h = 0.001f;
  float dxp = Sphere((vec3){.x = v.x + h, .y = v.y, .z = v.z});
  float dxm = Sphere((vec3){.x = v.x - h, .y = v.y, .z = v.z});
  float dyp = Sphere((vec3){.x = v.x, .y = v.y + h, .z = v.z});
  float dym = Sphere((vec3){.x = v.x, .y = v.y - h, .z = v.z});
  float dzp = Sphere((vec3){.x = v.x, .y = v.y, .z = v.z + h});
  float dzm = Sphere((vec3){.x = v.x, .y = v.y, .z = v.z - h});
  vec3 gradient = (vec3){.x = dxp - dxm, .y = dyp - dym, .z = dzp - dzm};
  gradient = vec3_old_skool_normalise(gradient);
  return gradient;
}

void manifold_octree_construct_base(struct ManifoldOctreeNode* octree_node, int size, float error, struct ArrayList* vertices);
void manifold_octree_destroy_octree(struct ManifoldOctreeNode* octree_node, struct ArrayList* collected_vertices);
void manifold_octree_generate_vertex_buffer(struct ManifoldOctreeNode* octree_node, struct Vector* vertices);
bool manifold_octree_construct_nodes(struct ManifoldOctreeNode* octree_node, struct ArrayList* vertices, int threads);
bool manifold_octree_construct_leaf(struct ManifoldOctreeNode* octree_node, struct ArrayList* vertices);
void manifold_octree_process_cell(struct ManifoldOctreeNode* octree_node, struct Vector* indexes, float threshold);
void manifold_octree_process_face(struct ManifoldOctreeNode* nodes[2], int direction, struct Vector* indexes, float threshold);
void manifold_octree_process_edge(struct ManifoldOctreeNode* nodes[4], int direction, struct Vector* indexes, float threshold);
void manifold_octree_process_indexes(struct ManifoldOctreeNode* nodes[4], int direction, struct Vector* indexes, float threshold);
void manifold_octree_cluster_cell_base(struct ManifoldOctreeNode* octree_node, float error);
void manifold_octree_cluster_cell(struct ManifoldOctreeNode* octree_node, float error);
void manifold_octree_gather_vertices(struct ManifoldOctreeNode* n, struct ArrayList* dest, int* surface_index);
void manifold_octree_cluster_face(struct ManifoldOctreeNode* nodes[2], int direction, int* surface_index, struct ArrayList* collected_vertices);
void manifold_octree_cluster_edge(struct ManifoldOctreeNode* nodes[4], int direction, int* surface_index, struct ArrayList* collected_vertices);
void manifold_octree_cluster_indexes(struct ManifoldOctreeNode* nodes[8], int direction, int* max_surface_index, struct ArrayList* collected_vertices);
void manifold_octree_assign_surface(struct ArrayList* vertices, int from, int to);

#endif  // MANIFOLD_OCTREE_H
