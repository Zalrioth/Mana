#pragma once
#ifndef VULKAN_CORE_H
#define VULKAN_CORE_H

#include "mana/core/memoryallocator.h"
//
#define _CRT_SECURE_NO_DEPRECATE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <cstorage/cstorage.h>
#include <stdalign.h>
#include <vulkan/vulkan.h>

#include "mana/core/corecommon.h"

#ifdef NDEBUG
static const bool enable_validation_layers = false;
#else
static const bool enable_validation_layers = true;
#endif

#define VULKAN_WAIT_SEMAPHORES 2
#define VALIDATION_LAYER_COUNT 1
static const char* const validation_layers[VALIDATION_LAYER_COUNT] = {"VK_LAYER_LUNARG_standard_validation"};
#define DEVICE_EXTENSION_COUNT 1
static const char* const device_extensions[DEVICE_EXTENSION_COUNT] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

enum VULKAN_CORE_STATUS { VULKAN_CORE_CREATE_WINDOW_ERROR = 0,
                          VULKAN_CORE_SUCCESS = 1,
                          VULKAN_CORE_CREATE_INSTANCE_ERROR = 2,
                          VULKAN_CORE_SETUP_DEBUG_MESSENGER_ERROR = 3,
                          VULKAN_CORE_CREATE_SURFACE_ERROR = 4,
                          VULKAN_CORE_PICK_PHYSICAL_DEVICE_ERROR = 5,
                          VULKAN_CORE_CREATE_LOGICAL_DEVICE_ERROR = 6,
                          VULKAN_CORE_CREATE_SWAP_CHAIN_ERROR = 7,
                          VULKAN_CORE_CREATE_IMAGE_VIEWS_ERROR = 8,
                          VULKAN_CORE_CREATE_RENDER_PASS_ERROR = 9,
                          VULKAN_CORE_CREATE_GRAPHICS_PIPELINE_ERROR = 10,
                          VULKAN_CORE_CREATE_FRAME_BUFFER_ERROR = 11,
                          VULKAN_CORE_CREATE_COMMAND_POOL_ERROR = 12,
                          VULKAN_CORE_CREATE_COMMAND_BUFFER_ERROR = 13,
                          VULKAN_CORE_CREATE_SYNC_OBJECT_ERROR = 14
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

int vulkan_core_init(struct VulkanState* vulkan_state);
void vulkan_command_pool_cleanup(struct VulkanState* vulkan_state);
int vulkan_core_create_instance(struct VulkanState* vulkan_state);
int vulkan_core_setup_debug_messenger(struct VulkanState* vulkan_state);
int vulkan_core_pick_physical_device(struct VulkanState* vulkan_state);
bool vulkan_core_can_device_render(struct VulkanState* vulkan_state, VkPhysicalDevice device);
int vulkan_core_create_logical_device(struct VulkanState* vulkan_state);
VkSampleCountFlagBits vulkan_core_get_max_usable_sample_count(struct VulkanState* vulkan_state);
int vulkan_core_create_command_pool(struct VulkanState* vulkan_state);
bool vulkan_core_check_validation_layer_support(struct VulkanState* vulkan_state);

#endif  // VULKAN_CORE_H
