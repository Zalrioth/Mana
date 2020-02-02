#pragma once
#ifndef DUAL_CONTOURING_H
#define DUAL_CONTOURING_H

#include "mana/graphics/dualcontouring/octree.h"
#include "mana/graphics/utilities/mesh.h"

struct DualContouring {
  int octree_size;
  struct Mesh *mesh;
  struct OctreeNode *head;
};

void dual_contorouring_init(struct DualContouring *dual_contouring, int octree_size);
void dual_contorouring_delete(struct DualContouring *dual_contouring);

#endif  // DUAL_CONTOURING_H
