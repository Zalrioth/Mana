#pragma once
#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/render/vulkanrenderer.h"

struct VulkanRenderer;

struct SwapChain {
  struct VkImage_T* swap_chain_images[MAX_SWAP_CHAIN_FRAMES];
  struct VkImageView_T* swap_chain_image_views[MAX_SWAP_CHAIN_FRAMES];
  struct VkFramebuffer_T* swap_chain_framebuffers[MAX_SWAP_CHAIN_FRAMES];

  struct VkRenderPass_T* render_pass;
};

int swapchain_init(struct SwapChain* swapchain, struct VulkanRenderer* vulkan_renderer);
void swapchain_delete(struct SwapChain* swapchain, struct VulkanRenderer* vulkan_renderer);

#endif  // SWAPCHAIN_H