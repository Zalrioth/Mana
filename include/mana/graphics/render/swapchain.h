#pragma once
#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/render/vulkanrenderer.h"

struct VulkanRenderer;

struct SwapChain {
  VkSemaphore image_available_semaphores[MAX_FRAMES_IN_FLIGHT];
  VkSemaphore render_finished_semaphores[MAX_FRAMES_IN_FLIGHT];
  VkFence in_flight_fences[MAX_FRAMES_IN_FLIGHT];
  VkSwapchainKHR swap_chain_khr;
  VkFormat swap_chain_image_format;
  VkExtent2D swap_chain_extent;
  VkRenderPass render_pass;
  size_t current_frame;

  VkCommandBuffer swap_chain_command_buffers[MAX_SWAP_CHAIN_FRAMES];
  VkImage swap_chain_images[MAX_SWAP_CHAIN_FRAMES];
  VkImageView swap_chain_image_views[MAX_SWAP_CHAIN_FRAMES];
  VkFramebuffer swap_chain_framebuffers[MAX_SWAP_CHAIN_FRAMES];
};

int swap_chain_init(struct SwapChain* swap_chain, struct VulkanRenderer* vulkan_renderer, int width, int height);
void swap_chain_delete(struct SwapChain* swap_chain, struct VulkanRenderer* vulkan_renderer);
int swap_chain_start(struct SwapChain* swap_chain, struct VulkanRenderer* vulkan_renderer, int swap_chain_num);
int swap_chain_stop(struct SwapChain* swap_chain, struct VulkanRenderer* vulkan_renderer, int swap_chain_num);

#endif  // SWAP_CHAIN_H