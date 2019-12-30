#pragma once
#ifndef BLIT_SWAPCHAIN_H
#define BLIT_SWAPCHAIN_H

#include "mana/graphics/shaders/shader.h"

struct BlitSwapchain {
  VkDescriptorSet descriptor_set;

  struct Mesh* image_mesh;

  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_buffer_memory;
  VkBuffer index_buffer;
  VkDeviceMemory index_buffer_memory;
};

int blit_swapchain_init(struct BlitSwapchain* blit_swapchain, struct VulkanRenderer* vulkan_renderer, struct Shader* shader);
void blit_swapchain_delete(struct BlitSwapchain* blit_swapchain, struct VulkanRenderer* vulkan_renderer);

#endif  // BLIT_SWAPCHAIN_H