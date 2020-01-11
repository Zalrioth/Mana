#pragma once
#ifndef BLIT_POST_PROCESS_H
#define BLIT_POST_PROCESS_H

#include <mana/graphics/shaders/blitshader.h>
#include "mana/graphics/shaders/shader.h"

struct BlitPostProcess {
  struct BlitShader blit_shader;

  VkDescriptorSet descriptor_set;

  struct Mesh* image_mesh;

  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_buffer_memory;
  VkBuffer index_buffer;
  VkDeviceMemory index_buffer_memory;
};

int blit_post_process_init(struct BlitPostProcess* blit_post_, struct VulkanRenderer* vulkan_renderer);
void blit_post_process_delete(struct BlitPostProcess* blit_post_process, struct VulkanRenderer* vulkan_renderer);
int blit_post_process_render(struct BlitPostProcess* blit_post_, struct VulkanRenderer* vulkan_renderer);

#endif  // BLIT_POST_PROCESS_H