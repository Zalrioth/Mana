#pragma once
#ifndef FULLSCREEN_QUAD_H
#define FULLSCREEN_QUAD_H

#include "mana/graphics/utilities/mesh.h"

struct FullscreenQuad {
  struct Mesh* mesh;
  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_buffer_memory;
  VkBuffer index_buffer;
  VkDeviceMemory index_buffer_memory;
};

void fullscreen_quad_init(struct FullscreenQuad* fullscreen_quad, struct VulkanRenderer* vulkan_renderer);
void fullscreen_quad_delete(struct FullscreenQuad* fullscreen_quad, struct VulkanRenderer* vulkan_renderer);

#endif  // FULLSCREEN_QUAD_H