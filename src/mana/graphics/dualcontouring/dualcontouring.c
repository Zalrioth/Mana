#include "mana/graphics/dualcontouring/dualcontouring.h"

#define MAX_THRESHOLDS 5

void dual_contorouring_init(struct DualContouring *dual_contouring, int octree_size) {
  const float THRESHOLDS[MAX_THRESHOLDS] = {-1.f, 0.1f, 1.f, 10.f, 50.f};
  int threshold_index = -1;
  threshold_index = (threshold_index + 1) % MAX_THRESHOLDS;

  dual_contouring->head = octree_build_octree((ivec3){-dual_contouring->octree_size / 2, -dual_contouring->octree_size / 2, -dual_contouring->octree_size / 2}, dual_contouring->octree_size, THRESHOLDS[threshold_index]);
}

void dual_contorouring_delete(struct DualContouring *dual_contouring) {
  octree_destroy_octree(dual_contouring->head);
  mesh_delete(dual_contouring->mesh);
}