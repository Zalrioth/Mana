#pragma once
#ifndef SPRITE_H
#define SPRITE_H

#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/utilities/texture.h"

struct Sprite {
  struct Mesh* image_mesh;
  struct Texture* image_texture;

  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_buffer_memory;
  VkBuffer index_buffer;
  VkDeviceMemory index_buffer_memory;
  VkBuffer uniform_buffer;
  VkDeviceMemory uniform_buffers_memory;
  VkDescriptorSet descriptor_set;
};

enum {
  SPRITE_SUCCESS = 1
};

int sprite_init(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer);
void sprite_delete(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer);
int sprite_create_vertex_buffer(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer);
int sprite_create_index_buffer(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer);
int sprite_create_uniform_buffers(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer);

void vulkan_index_buffer_cleanup(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer);
void vulkan_vertex_buffer_cleanup(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer);
void vulkan_texture_cleanup(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer);

#endif  // SPRITE_H
