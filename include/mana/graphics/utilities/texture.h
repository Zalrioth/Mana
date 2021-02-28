#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include "mana/core/memoryallocator.h"
//
#include <stb_image.h>

#include "mana/core/gpuapi.h"
#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/utilities/graphicsutils.h"

struct VulkanState;

enum FilterType {
  FILTER_NEAREST = 0,
  FILTER_LINEAR
};

enum ModeType {
  MODE_REPEAT = 0,
  MODE_MIRRORED_REPEAT,
  MODE_CLAMP_TO_EDGE,
  MODE_CLAMP_TO_BORDER
};

struct TextureSettings {
  char *path;
  enum FilterType filter_type;
  enum ModeType mode_type;
  int mip_maps_enabled;
  int premultiplied_alpha;  // Does the image already have premultiplied alphas, 0: No 1: Yes
};

struct Texture {
  // TODO: Will probably need to use this later, maybe global static int that increments after each texture creation
  unsigned int id;
  char *name;
  char *type;
  char *path;
  VkImage texture_image;                // Raw image data in ram
  VkDeviceMemory texture_image_memory;  // Image data in GPU/device ram
  VkImageView texture_image_view;       // Texture format
  VkSampler texture_sampler;            // Image sampling settings
  //VkFilter filter_type;
  int width;
  int height;
};

int texture_init(struct Texture *texture, struct GPUAPI *gpu_api, struct TextureSettings texture_settings);
void texture_delete(struct Texture *texture, struct GPUAPI *gpu_api);
void texture_copy_buffer_to_image(struct GPUAPI *gpu_api, VkBuffer *buffer, VkImage *image, uint32_t width, uint32_t height);

#endif  // TEXTURE_H
