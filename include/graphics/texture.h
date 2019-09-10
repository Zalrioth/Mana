#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include "core/common.h"
#include "core/window.h"
#include "graphics/graphicsutils.h"

struct Texture {
  // TODO: Will probably need to use this later, maybe global static int that increments after each texture creation
  unsigned int id;
  char *type;
  char *path;
  VkImage textureImage;               // Raw image data in ram
  VkDeviceMemory textureImageMemory;  // Image data in GPU/device ram
  VkImageView textureImageView;       // Texture format
  VkSampler textureSampler;           // Image sampling settings
};

// Forward declaration of Window struct
struct Window;

void texture_init(struct Texture *texture, char *path);
void texture_delete(struct Window *window, struct Texture *texture);
int texture_create_image(struct Window *window, struct Texture *texture);
int texture_create_sampler(struct Window *window, struct Texture *texture);
int texture_create_texture_image_view(struct Window *window, struct Texture *texture);
int texture_transition_image_layout(struct Window *window, VkImage *image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
void texture_copy_buffer_to_image(struct Window *window, VkBuffer *buffer, VkImage *image, uint32_t width, uint32_t height);
uint32_t texture_find_memory_type(struct Window *window, uint32_t typeFilter, VkMemoryPropertyFlags properties);

#endif  // TEXTURE_H
