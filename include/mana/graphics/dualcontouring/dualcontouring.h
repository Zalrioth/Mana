#pragma once
#ifndef DUAL_CONTOURING_H
#define DUAL_CONTOURING_H

#include "mana/graphics/dualcontouring/octree.h"
#include "mana/graphics/utilities/mesh.h"

struct DualContouringUniformBufferObject {
  alignas(16) mat4 model;
  alignas(16) mat4 view;
  alignas(16) mat4 proj;
};

struct DualContouring {
  int octree_size;
  struct OctreeNode *head;

  struct Mesh *mesh;
  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_buffer_memory;
  VkBuffer index_buffer;
  VkDeviceMemory index_buffer_memory;
  VkBuffer uniform_buffer;
  VkDeviceMemory uniform_buffers_memory;
  VkDescriptorSet descriptor_set;
};

int dual_contouring_init(struct DualContouring *dual_contouring, struct VulkanRenderer *vulkan_renderer, int octree_size, struct Shader *shader);
void dual_contouring_delete(struct DualContouring *dual_contouring, struct VulkanRenderer *vulkan_renderer);

#endif  // DUAL_CONTOURING_H
