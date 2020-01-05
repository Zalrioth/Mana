#pragma once
#ifndef POST_PROCESS_H
#define POST_PROCESS_H

#include <stdbool.h>
#include "mana/graphics/render/vulkanrenderer.h"

struct PostProcess {
  struct VkFramebuffer_T* post_process_framebuffers[2];
  VkRenderPass render_passes[2];
  VkSemaphore post_process_semaphores[2];
  VkSampler texture_sampler;

  struct VkImage_T* color_images[2];
  struct VkDeviceMemory_T* color_image_memories[2];
  struct VkImageView_T* color_image_views[2];

  bool ping_pong;
};

int post_process_init(struct PostProcess* post_process, struct VulkanRenderer* vulkan_renderer);
int post_process_delete(struct PostProcess* post_process, struct VulkanRenderer* vulkan_renderer);

#endif  // POST_PROCESS_H