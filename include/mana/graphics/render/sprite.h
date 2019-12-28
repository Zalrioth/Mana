#pragma once
#ifndef SPRITE_H
#define SPRITE_H

#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/texture.h"

struct Sprite {
  struct Mesh* image_mesh;
  struct Texture* image_texture;

  struct VkBuffer_T* vertex_buffer;
  struct VkDeviceMemory_T* vertex_buffer_memory;
  struct VkBuffer_T* index_buffer;
  struct VkDeviceMemory_T* index_buffer_memory;
  struct VkBuffer_T* uniform_buffer;
  struct VkDeviceMemory_T* uniform_buffers_memory;
  struct VkDescriptorSet_T* descriptor_set;
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
