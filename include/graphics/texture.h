#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include "core/common.h"
#include "core/vulkanrenderer.h"
#include "graphics/graphicsutils.h"

struct VulkanRenderer;

struct Texture {
  // TODO: Will probably need to use this later, maybe global static int that increments after each texture creation
  unsigned int id;
  char *type;
  char *path;
  VkImage texture_image;                // Raw image data in ram
  VkDeviceMemory texture_image_memory;  // Image data in GPU/device ram
  VkImageView texture_image_view;       // Texture format
  VkSampler texture_sampler;            // Image sampling settings
};

void texture_init(struct Texture *texture, char *path);
void texture_delete(struct VulkanRenderer *vulkan_renderer, struct Texture *texture);
int texture_create_image(struct VulkanRenderer *vulkan_renderer, struct Texture *texture);
int texture_create_sampler(struct VulkanRenderer *vulkan_renderer, struct Texture *texture);
int texture_create_texture_image_view(struct VulkanRenderer *vulkan_renderer, struct Texture *texture);
int texture_transition_image_layout(struct VulkanRenderer *vulkan_renderer, VkImage *image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
void texture_copy_buffer_to_image(struct VulkanRenderer *vulkan_renderer, VkBuffer *buffer, VkImage *image, uint32_t width, uint32_t height);
uint32_t texture_find_memory_type(struct VulkanRenderer *vulkan_renderer, uint32_t typeFilter, VkMemoryPropertyFlags properties);

#endif  // TEXTURE_H
