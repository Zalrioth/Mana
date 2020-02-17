#pragma once
#ifndef OCTREE_H
#define OCTREE_H

#include "mana/core/memoryallocator.h"
//
#include <cglm/cglm.h>
#include <cnoise/cnoise.h>

#include "mana/graphics/dualcontouring/qef.h"
#include "mana/graphics/utilities/mesh.h"

enum OctreeNodeType {
  NODE_NONE,
  NODE_INTERNAL,
  NODE_PSUEDO,
  NODE_LEAF,
};

struct OctreeDrawInfo {
  int index;
  int corners;
  vec3 position;
  vec3 average_normal;
  struct QefData qef;
};

void octree_draw_info_init(struct OctreeDrawInfo* octree_draw_info);

struct OctreeNode {
  enum OctreeNodeType type;
  ivec3 min;
  int size;
  struct OctreeNode* children[8];
  struct OctreeDrawInfo* draw_info;
};

void octree_init(struct OctreeNode* octree_node, enum OctreeNodeType type);
void octree_init_none(struct OctreeNode* octree_node);
struct OctreeNode* octree_simplify_octree(struct OctreeNode* node, float threshold);
void octree_generate_vertex_indices(struct OctreeNode* node, struct Mesh* mesh);
void octree_contour_process_edge(struct OctreeNode* node[4], int dir, struct Mesh* mesh);
void octree_contour_edge_proc(struct OctreeNode* node[4], int dir, struct Mesh* mesh);
void octree_contour_face_proc(struct OctreeNode* node[2], int dir, struct Mesh* mesh);
void octree_contour_cell_proc(struct OctreeNode* node, struct Mesh* mesh);
void octree_approximate_zero_crossing_position(const vec3 p0, const vec3 p1, vec3 dest);
void octree_calculate_surface_normal(const vec3 p, vec3 dest);
struct OctreeNode* octree_construct_leaf(struct OctreeNode* leaf, float* noise_set);
struct OctreeNode* octree_construct_octree_nodes(struct OctreeNode* node, float* noise_set);
struct OctreeNode* octree_build_octree(const ivec3 min, const int size, const float threshold, float* noise_set);
void octree_generate_mesh_from_octree(struct OctreeNode* node, struct Mesh* mesh);
void octree_destroy_octree(struct OctreeNode* node);

#endif  // OCTREE_H
