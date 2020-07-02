#include "mana/graphics/render/vulkanrenderer.h"

static void vulkan_renderer_surface_cleanup(struct VulkanState* vulkan_state);
static VkSampleCountFlagBits vulkan_renderer_get_max_usable_sample_count(struct VulkanState* vulkan_state);
static bool vulkan_renderer_device_can_present(struct VulkanState* vulkan_state, VkPhysicalDevice device);

// TODO: Add error checking
int vulkan_renderer_init(struct VulkanState* vulkan_state, int width, int height) {
  vulkan_state->swap_chain = malloc(sizeof(struct SwapChain));
  vulkan_state->gbuffer = malloc(sizeof(struct GBuffer));
  vulkan_state->post_process = malloc(sizeof(struct PostProcess));
  vulkan_state->msaa_samples = vulkan_renderer_get_max_usable_sample_count(vulkan_state);

  // TODO: If device cannot render, check for new device that can then recreate core?
  if (!vulkan_renderer_device_can_present(vulkan_state, vulkan_state->physical_device))
    return VULKAN_RENDERER_NO_PRESENTABLE_DEVICE_ERROR;

  swap_chain_init(vulkan_state->swap_chain, vulkan_state, width, height);
  post_process_init(vulkan_state->post_process, vulkan_state);
  gbuffer_init(vulkan_state->gbuffer, vulkan_state);

  // Maybe move this
  blit_swap_chain_init(vulkan_state->swap_chain->blit_swap_chain, vulkan_state);
  blit_post_process_init(vulkan_state->post_process->blit_post_process, vulkan_state);

  return VULKAN_RENDERER_SUCCESS;
}

void vulkan_renderer_delete(struct VulkanState* vulkan_state) {
  post_process_delete(vulkan_state->post_process, vulkan_state);
  free(vulkan_state->post_process);
  gbuffer_delete(vulkan_state->gbuffer, vulkan_state);
  free(vulkan_state->gbuffer);
  swap_chain_delete(vulkan_state->swap_chain, vulkan_state);
  free(vulkan_state->swap_chain);
  vulkan_renderer_surface_cleanup(vulkan_state);
}

static void vulkan_renderer_surface_cleanup(struct VulkanState* vulkan_state) {
  vkDestroySurfaceKHR(vulkan_state->instance, vulkan_state->surface, NULL);
}
//window->engine->graphics_library.glfw_library
void vulkan_renderer_recreate_swap_chain(struct VulkanState* vulkan_state, struct GraphicsLibrary* graphics_library, int* width_handle, int* height_handle) {
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

  // When window is minimized will pause thread and gpu
  vkDeviceWaitIdle(vulkan_state->device);

  post_process_delete(vulkan_state->post_process, vulkan_state);
  gbuffer_delete(vulkan_state->gbuffer, vulkan_state);
  swap_chain_delete(vulkan_state->swap_chain, vulkan_state);
  swap_chain_init(vulkan_state->swap_chain, vulkan_state, width, height);
  post_process_init(vulkan_state->post_process, vulkan_state);
  gbuffer_init(vulkan_state->gbuffer, vulkan_state);

  blit_swap_chain_init(vulkan_state->swap_chain->blit_swap_chain, vulkan_state);
  blit_post_process_init(vulkan_state->post_process->blit_post_process, vulkan_state);

  //create_swap_chain(vulkan_state, width, height);
  //create_image_views(vulkan_state);
  //create_render_pass(vulkan_state);
  //create_graphics_pipeline(vulkan_state);
  //create_depth_resources(vulkan_state);
  //create_framebuffers(vulkan_state);
  //create_command_buffers(vulkan_state);
}

static bool vulkan_renderer_device_can_present(struct VulkanState* vulkan_state, VkPhysicalDevice device) {
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

  VkQueueFamilyProperties* queue_families = malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

  bool present_family_found = false;
  for (int queue_family_num = 0; queue_family_num < queue_family_count; queue_family_num++) {
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, queue_family_num, vulkan_state->surface, &present_support);

    if (queue_families[queue_family_num].queueCount > 0 && present_support) {
      (&vulkan_state->indices)->present_family = queue_family_num;
      present_family_found = true;
      break;
    }
  }

  free(queue_families);

  if (!present_family_found)
    return false;

  return true;
}

static VkSampleCountFlagBits vulkan_renderer_get_max_usable_sample_count(struct VulkanState* vulkan_state) {
  //return VK_SAMPLE_COUNT_1_BIT;
  VkPhysicalDeviceProperties physical_device_properties;
  vkGetPhysicalDeviceProperties(vulkan_state->physical_device, &physical_device_properties);

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
