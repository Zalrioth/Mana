#pragma once
#ifndef BLIT_POST_PROCESS_H
#define BLIT_POST_PROCESS_H

#include <mana/graphics/shaders/blitshader.h>
#include "mana/graphics/shaders/shader.h"

struct BlitSwapChain {
  struct BlitShader blit_shader;

  VkDescriptorSet descriptor_set;

  struct Mesh* image_mesh;

  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_buffer_memory;
  VkBuffer index_buffer;
  VkDeviceMemory index_buffer_memory;
};

int blit_swapchain_init(struct BlitSwapChain* blit_swapchain, struct VulkanRenderer* vulkan_renderer);
void blit_swapchain_delete(struct BlitSwapChain* blit_swapchain, struct VulkanRenderer* vulkan_renderer);

#endif  // BLIT_SWAPCHAIN_H