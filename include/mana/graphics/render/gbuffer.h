#pragma once
#ifndef G_BUFFER_H
#define G_BUFFER_H

#include "mana/graphics/render/vulkanrenderer.h"

struct VulkanRenderer;

struct GBuffer {
  VkFramebuffer gbuffer_framebuffer;
  VkRenderPass render_pass;
  VkSampler texture_sampler;
  VkSemaphore gbuffer_semaphore;

  VkImage color_image;
  VkDeviceMemory color_image_memory;
  VkImageView color_image_view;

  VkImage normal_image;
  VkDeviceMemory normal_image_memory;
  VkImageView normal_image_view;

  VkImage depth_image;
  VkDeviceMemory depth_image_memory;
  VkImageView depth_image_view;

  mat4 projection_matrix;
  mat4 view_matrix;
};

int gbuffer_init(struct GBuffer* gbuffer, struct VulkanRenderer* vulkan_renderer);
void gbuffer_delete(struct GBuffer* gbuffer, struct VulkanRenderer* vulkan_renderer);

#endif  // G_BUFFER_H