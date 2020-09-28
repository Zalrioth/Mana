#include "mana/graphics/render/vulkanrenderer.h"

static void vulkan_renderer_surface_cleanup(struct GPUAPI* gpu_api);
static VkSampleCountFlagBits vulkan_renderer_get_max_usable_sample_count(struct GPUAPI* gpu_api);
static bool vulkan_renderer_device_can_present(struct GPUAPI* gpu_api, VkPhysicalDevice device);

// TODO: Add error checking
int vulkan_renderer_init(struct GPUAPI* gpu_api, int width, int height) {
  gpu_api->vulkan_state->swap_chain = malloc(sizeof(struct SwapChain));
  gpu_api->vulkan_state->gbuffer = malloc(sizeof(struct GBuffer));
  gpu_api->vulkan_state->post_process = malloc(sizeof(struct PostProcess));
  gpu_api->vulkan_state->msaa_samples = vulkan_renderer_get_max_usable_sample_count(gpu_api);

  // TODO: If device cannot render, check for new device that can then recreate core?
  if (!vulkan_renderer_device_can_present(gpu_api, gpu_api->vulkan_state->physical_device))
    return VULKAN_RENDERER_NO_PRESENTABLE_DEVICE_ERROR;

  swap_chain_init(gpu_api->vulkan_state->swap_chain, gpu_api, width, height);
  post_process_init(gpu_api->vulkan_state->post_process, gpu_api);
  gbuffer_init(gpu_api->vulkan_state->gbuffer, gpu_api->vulkan_state);

  // Maybe move this
  blit_swap_chain_init(gpu_api->vulkan_state->swap_chain->blit_swap_chain, gpu_api);
  blit_post_process_init(gpu_api->vulkan_state->post_process->blit_post_process, gpu_api);

  return VULKAN_RENDERER_SUCCESS;
}

void vulkan_renderer_delete(struct GPUAPI* gpu_api) {
  post_process_delete(gpu_api->vulkan_state->post_process, gpu_api);
  free(gpu_api->vulkan_state->post_process);
  gbuffer_delete(gpu_api->vulkan_state->gbuffer, gpu_api->vulkan_state);
  free(gpu_api->vulkan_state->gbuffer);
  swap_chain_delete(gpu_api->vulkan_state->swap_chain, gpu_api);
  free(gpu_api->vulkan_state->swap_chain);
  vulkan_renderer_surface_cleanup(gpu_api);
}

static void vulkan_renderer_surface_cleanup(struct GPUAPI* gpu_api) {
  vkDestroySurfaceKHR(gpu_api->vulkan_state->instance, gpu_api->vulkan_state->surface, NULL);
}

void vulkan_renderer_recreate_swap_chain(struct GPUAPI* gpu_api, struct GraphicsLibrary* graphics_library, int* width_handle, int* height_handle) {
  int width = 0, height = 0;
  switch (graphics_library->type) {
    case (NO_LIBRARY):
      break;
    case (GLFW_LIBRARY):
      while (width == 0 || height == 0) {
        glfwGetFramebufferSize(graphics_library->glfw_library.glfw_window, &width, &height);
        glfwWaitEvents();
      }
    case (MOLTENVK_LIBRARY):
      break;
  }

  *width_handle = width;
  *height_handle = height;

  // NOTE: When window is minimized will pause thread and gpu
  vkDeviceWaitIdle(gpu_api->vulkan_state->device);

  post_process_delete(gpu_api->vulkan_state->post_process, gpu_api);
  gbuffer_delete(gpu_api->vulkan_state->gbuffer, gpu_api->vulkan_state);
  swap_chain_delete(gpu_api->vulkan_state->swap_chain, gpu_api);

  swap_chain_init(gpu_api->vulkan_state->swap_chain, gpu_api, width, height);
  post_process_init(gpu_api->vulkan_state->post_process, gpu_api);
  gbuffer_init(gpu_api->vulkan_state->gbuffer, gpu_api->vulkan_state);

  blit_swap_chain_init(gpu_api->vulkan_state->swap_chain->blit_swap_chain, gpu_api);
  blit_post_process_init(gpu_api->vulkan_state->post_process->blit_post_process, gpu_api);
}

static bool vulkan_renderer_device_can_present(struct GPUAPI* gpu_api, VkPhysicalDevice device) {
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

  VkQueueFamilyProperties* queue_families = malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

  bool present_family_found = false;
  for (int queue_family_num = 0; queue_family_num < queue_family_count; queue_family_num++) {
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, queue_family_num, gpu_api->vulkan_state->surface, &present_support);

    if (queue_families[queue_family_num].queueCount > 0 && present_support) {
      (&gpu_api->vulkan_state->indices)->present_family = queue_family_num;
      present_family_found = true;
      break;
    }
  }

  free(queue_families);

  if (!present_family_found)
    return false;

  return true;
}

static VkSampleCountFlagBits vulkan_renderer_get_max_usable_sample_count(struct GPUAPI* gpu_api) {
  //return VK_SAMPLE_COUNT_1_BIT;
  VkPhysicalDeviceProperties physical_device_properties = {0};
  vkGetPhysicalDeviceProperties(gpu_api->vulkan_state->physical_device, &physical_device_properties);

  VkSampleCountFlags counts = physical_device_properties.limits.framebufferColorSampleCounts & physical_device_properties.limits.framebufferDepthSampleCounts;
  if (counts & VK_SAMPLE_COUNT_64_BIT)
    return VK_SAMPLE_COUNT_64_BIT;
  if (counts & VK_SAMPLE_COUNT_32_BIT)
    return VK_SAMPLE_COUNT_32_BIT;
  if (counts & VK_SAMPLE_COUNT_16_BIT)
    return VK_SAMPLE_COUNT_16_BIT;
  if (counts & VK_SAMPLE_COUNT_8_BIT)
    return VK_SAMPLE_COUNT_8_BIT;
  if (counts & VK_SAMPLE_COUNT_4_BIT)
    return VK_SAMPLE_COUNT_4_BIT;
  if (counts & VK_SAMPLE_COUNT_2_BIT)
    return VK_SAMPLE_COUNT_2_BIT;

  return VK_SAMPLE_COUNT_1_BIT;
}
