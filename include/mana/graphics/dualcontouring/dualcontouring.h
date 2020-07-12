#pragma once
#ifndef DUAL_CONTOURING_H
#define DUAL_CONTOURING_H

#include "mana/core/memoryallocator.h"
//
#include <tinycthread.h>

#include "mana/core/corecommon.h"
#include "mana/graphics/dualcontouring/octree.h"
#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/utilities/mesh.h"

struct DualContouringUniformBufferObject {
  alignas(32) mat4 model;
  alignas(32) mat4 view;
  alignas(32) mat4 proj;
  alignas(32) vec3 camera_pos;
};

struct DualContouring {
  int octree_size;
  struct OctreeNode *head;
  struct Vector *noises;
  float (*density_func_single)(struct Vector *, float, float, float);
  float *(*density_func_set)(struct Vector *, float, float, float, int, int, int);
  float *noise_set;

  struct Mesh *mesh;
  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_buffer_memory;
  VkBuffer index_buffer;
  VkDeviceMemory index_buffer_memory;
  VkBuffer dc_uniform_buffer;
  VkDeviceMemory dc_uniform_buffer_memory;
  VkBuffer lighting_uniform_buffer;
  VkDeviceMemory lighting_uniform_buffer_memory;
  VkDescriptorSet descriptor_set;
};

int dual_contouring_init(struct DualContouring *dual_contouring, struct VulkanState *vulkan_renderer, int octree_size, struct Shader *shader, struct Vector *noises, float (*density_func_single)(struct Vector *, float, float, float), float *(*density_func_set)(struct Vector *, float, float, float, int, int, int));
void dual_contouring_delete(struct DualContouring *dual_contouring, struct VulkanState *vulkan_renderer);

#endif  // DUAL_CONTOURING_H
