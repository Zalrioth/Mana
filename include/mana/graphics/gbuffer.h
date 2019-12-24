#pragma once
#ifndef G_BUFFER_H
#define G_BUFFER_H

#include "mana/core/common.h"
#include "mana/core/vulkanrenderer.h"

struct VulkanRenderer;

struct GBuffer {
  struct VkImage_T* swap_chain_images[MAX_SWAP_CHAIN_FRAMES];
  struct VkImageView_T* swap_chain_image_views[MAX_SWAP_CHAIN_FRAMES];
  struct VkFramebuffer_T* swap_chain_framebuffers[MAX_SWAP_CHAIN_FRAMES];

  struct VkImage_T* depth_image;
  struct VkDeviceMemory_T* depth_image_memory;
  struct VkImageView_T* depth_image_view;

  struct VkImage_T* normal_image;
  struct VkDeviceMemory_T* normal_image_memory;
  struct VkImageView_T* normal_image_view;

  struct VkRenderPass_T* render_pass;
};

int gbuffer_init(struct GBuffer* gbuffer, struct VulkanRenderer* vulkan_renderer);
void gbuffer_delete(struct GBuffer* gbuffer, struct VulkanRenderer* vulkan_renderer);

#endif  // G_BUFFER_H