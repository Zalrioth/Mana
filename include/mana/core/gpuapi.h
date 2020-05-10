#pragma once
#ifndef GPU_API_H
#define GPU_API_H

#include "mana/core/memoryallocator.h"
//
#include <vulkan/vulkan.h>

#include "mana/core/vulkancore.h"
#include "mana/graphics/render/vulkanrenderer.h"

struct GraphicsLibrary;

enum GPU_API_STATUS {
  GPU_API_SUCCESS = 0,
  GPU_API_VULKAN_ERROR,
  GPU_API_LAST_ERROR
};

enum APIType { VULKAN_API = 0 };

struct GPUAPI {
  union {
    struct VulkanState* vulkan_state;
  };
  enum APIType type;
};

int gpu_api_init(struct GPUAPI* gpu_api, enum APIType gpu_api_type, struct GraphicsLibrary* graphics_library, const char** graphics_lbrary_extensions, uint32_t* graphics_library_extension_count);

#endif  // GPU_API_H
