#pragma once
#ifndef VULKAN_CORE_H
#define VULKAN_CORE_H

#include "mana/core/memoryallocator.h"
//
#include <cstorage/cstorage.h>
#include <stdalign.h>
#include <ubermath/ubermath.h>
#include <vulkan/vulkan.h>

#include "mana/core/corecommon.h"

#ifdef NDEBUG
static const bool enable_validation_layers = false;
#else
static const bool enable_validation_layers = true;
#endif

#define VULKAN_WAIT_SEMAPHORES 2
#define VULKAN_VALIDATION_LAYER_COUNT 1
static const char* const validation_layers[VULKAN_VALIDATION_LAYER_COUNT] = {"VK_LAYER_LUNARG_standard_validation"};
#define VULKAN_DEVICE_EXTENSION_COUNT 1
static const char* const device_extensions[VULKAN_DEVICE_EXTENSION_COUNT] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

enum VULKAN_CORE_STATUS {
  VULKAN_CORE_SUCCESS = 0,
  VULKAN_CORE_CREATE_INSTANCE_ERROR,
  VULKAN_CORE_SETUP_DEBUG_MESSENGER_ERROR,
  VULKAN_CORE_PICK_PHYSICAL_DEVICE_ERROR,
  VULKAN_CORE_CREATE_LOGICAL_DEVICE_ERROR,
  VULKAN_CORE_CREATE_COMMAND_POOL_ERROR,
  VULKAN_CORE_LAST_ERROR
};

struct QueueFamilyIndices {
  uint32_t graphics_family;
  uint32_t present_family;
};

struct VulkanState {
  VkInstance instance;
  VkSurfaceKHR surface;
  VkPhysicalDevice physical_device;
  VkDevice device;
  VkQueue graphics_queue;
  VkQueue present_queue;
  VkDebugUtilsMessengerEXT debug_messenger;
  VkCommandPool command_pool;
  VkSampleCountFlagBits msaa_samples;
  struct QueueFamilyIndices indices;
  bool framebuffer_resized;
  struct SwapChain* swap_chain;
  struct GBuffer* gbuffer;
  struct PostProcess* post_process;
};

int vulkan_core_init(struct VulkanState* vulkan_state, const char** graphics_lbrary_extensions, uint32_t* graphics_library_extension_count);
void vulkan_core_delete(struct VulkanState* vulkan_state);

#endif  // VULKAN_CORE_H
