#pragma once
#ifndef BLIT_SWAP_CHAIN_H
#define BLIT_SWAP_CHAIN_H

#include <mana/graphics/shaders/blitshader.h>
#include "mana/graphics/shaders/shader.h"

struct BlitSwapChain {
  struct BlitShader blit_shader;
  struct Mesh* image_mesh;
  // Currently two for each post process ping pong, probably more needed later
  VkDescriptorSet descriptor_sets[2];
  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_buffer_memory;
  VkBuffer index_buffer;
  VkDeviceMemory index_buffer_memory;
};

int blit_swap_chain_init(struct BlitSwapChain* blit_swapchain, struct VulkanRenderer* vulkan_renderer);
void blit_swap_chain_delete(struct BlitSwapChain* blit_swapchain, struct VulkanRenderer* vulkan_renderer);
int blit_swap_chain_render(struct BlitSwapChain* blit_swapchain, struct VulkanRenderer* vulkan_renderer);

#endif  // BLIT_SWAP_CHAIN_H