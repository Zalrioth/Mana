#pragma once
#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include "mana/core/memoryallocator.h"
//
#define _CRT_SECURE_NO_DEPRECATE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstorage/cstorage.h>
#include <stdalign.h>
#include <ubermath/ubermath.h>
#include <vulkan/vulkan.h>

#include "mana/core/corecommon.h"
#include "mana/core/graphicslibrary.h"
#include "mana/core/vulkancore.h"
#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/render/gbuffer.h"
#include "mana/graphics/render/postprocess.h"
#include "mana/graphics/render/swapchain.h"
#include "mana/graphics/utilities/graphicsutils.h"

struct GraphicsLibrary;

enum VULKAN_RENDERER_STATUS {
  VULKAN_RENDERER_SUCCESS = 0,
  VULKAN_RENDERER_NO_PRESENTABLE_DEVICE_ERROR,
  VULKAN_RENDERER_CREATE_WINDOW_ERROR,
  VULKAN_RENDERER_CREATE_INSTANCE_ERROR,
  VULKAN_RENDERER_SETUP_DEBUG_MESSENGER_ERROR,
  VULKAN_RENDERER_CREATE_SURFACE_ERROR,
  VULKAN_RENDERER_PICK_PHYSICAL_DEVICE_ERROR,
  VULKAN_RENDERER_CREATE_LOGICAL_DEVICE_ERROR,
  VULKAN_RENDERER_CREATE_SWAP_CHAIN_ERROR,
  VULKAN_RENDERER_CREATE_IMAGE_VIEWS_ERROR,
  VULKAN_RENDERER_CREATE_RENDER_PASS_ERROR,
  VULKAN_RENDERER_CREATE_GRAPHICS_PIPELINE_ERROR,
  VULKAN_RENDERER_CREATE_FRAME_BUFFER_ERROR,
  VULKAN_RENDERER_CREATE_COMMAND_POOL_ERROR,
  VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR,
  VULKAN_RENDERER_CREATE_SYNC_OBJECT_ERROR,
  VULKAN_RENDERER_LAST_ERROR
};

int vulkan_renderer_init(struct GPUAPI* gpu_api, int width, int height, int msaa_samples);
void vulkan_renderer_delete(struct GPUAPI* gpu_api);
void vulkan_renderer_recreate_swap_chain(struct GPUAPI* gpu_api, struct GraphicsLibrary* graphics_library, int* width_handle, int* height_handle);

#endif  // VULKAN_RENDERER_H
