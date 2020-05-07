#include "mana/core/gpuapi.h"

int gpu_api_init(struct GPUAPI* gpu_api, enum APIType gpu_api_type, struct GraphicsLibrary* graphics_library) {
  gpu_api->type = gpu_api_type;
  gpu_api->vulkan_state = calloc(1, sizeof(struct VulkanState));
  switch (gpu_api->type) {
    case (VULKAN_API):
      vulkan_core_init(gpu_api->vulkan_state);
      break;
  }

  return 1;
}