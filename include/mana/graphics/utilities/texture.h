#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include "mana/core/memoryallocator.h"
//
#include <stb_image.h>

#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/render/vulkanrenderer.h"
#include "mana/graphics/utilities/graphicsutils.h"

struct VulkanState;

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

int texture_init(struct Texture *texture, struct VulkanState *vulkan_renderer, char *path, VkFilter filter);
void texture_delete(struct VulkanState *vulkan_renderer, struct Texture *texture);
void texture_copy_buffer_to_image(struct VulkanState *vulkan_renderer, VkBuffer *buffer, VkImage *image, uint32_t width, uint32_t height);

#endif  // TEXTURE_H
