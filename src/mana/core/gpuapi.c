#include "mana/core/gpuapi.h"

int gpu_api_init(struct GPUAPI* gpu_api, enum APIType gpu_api_type, struct GraphicsLibrary* graphics_library) {
  gpu_api->type = gpu_api_type;
  gpu_api->vulkan_state = calloc(1, sizeof(struct VulkanState));
  switch (gpu_api->type) {
    case (VULKAN_API):
      int vulkan_core_error = vulkan_core_init(gpu_api->vulkan_state);
      switch (vulkan_core_error) {
        case (VULKAN_CORE_SUCCESS):
          break;
        case (VULKAN_CORE_CREATE_INSTANCE_ERROR):
          fprintf(stderr, "Failed to create a Vulkan core instance!\n");
          return GPU_API_VULKAN_ERROR;
        case (VULKAN_CORE_SETUP_DEBUG_MESSENGER_ERROR):
          fprintf(stderr, "Failed to set up Vulkan debug core messenger!\n");
          return GPU_API_VULKAN_ERROR;
        case (VULKAN_CORE_PICK_PHYSICAL_DEVICE_ERROR):
          fprintf(stderr, "Failed to find a suitable GPU for Vulkan core!\n");
          return GPU_API_VULKAN_ERROR;
        case (VULKAN_CORE_CREATE_LOGICAL_DEVICE_ERROR):
          fprintf(stderr, "Failed to create a Vulkan core logical device!\n");
          return GPU_API_VULKAN_ERROR;
        case (VULKAN_CORE_CREATE_COMMAND_POOL_ERROR):
          fprintf(stderr, "Failed to create Vulkan core command pool!\n");
          return GPU_API_VULKAN_ERROR;
        default:
          fprintf(stderr, "Unknown Vulkan core error! Error code: %d\n", vulkan_core_error);
          return GPU_API_VULKAN_ERROR;
      }
      break;
  }

  return GPU_API_SUCCESS;
}