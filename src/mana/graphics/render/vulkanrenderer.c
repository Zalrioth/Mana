#include "mana/graphics/render/vulkanrenderer.h"

static bool vulkan_renderer_device_can_present(struct VulkanState* vulkan_state, VkPhysicalDevice device);
static VkSampleCountFlagBits vulkan_renderer_get_max_usable_sample_count(struct VulkanState* vulkan_state);

int vulkan_renderer_init(struct VulkanState* vulkan_state, int width, int height) {
  vulkan_state->swap_chain = malloc(sizeof(struct SwapChain));
  vulkan_state->gbuffer = malloc(sizeof(struct GBuffer));
  vulkan_state->post_process = malloc(sizeof(struct PostProcess));
  vulkan_state->msaa_samples = vulkan_renderer_get_max_usable_sample_count(vulkan_state);

  vulkan_renderer_device_can_present(vulkan_state, vulkan_state->physical_device);

  swap_chain_init(vulkan_state->swap_chain, vulkan_state, width, height);
  post_process_init(vulkan_state->post_process, vulkan_state);
  gbuffer_init(vulkan_state->gbuffer, vulkan_state);

  // Maybe move this
  blit_swap_chain_init(vulkan_state->swap_chain->blit_swap_chain, vulkan_state);
  blit_post_process_init(vulkan_state->post_process->blit_post_process, vulkan_state);

  return VULKAN_RENDERER_SUCCESS;
}

static bool vulkan_renderer_device_can_present(struct VulkanState* vulkan_state, VkPhysicalDevice device) {
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

  struct Vector queue_families = {0};
  memset(&queue_families, 0, sizeof(queue_families));
  vector_init(&queue_families, sizeof(VkQueueFamilyProperties));
  vector_resize(&queue_families, queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.items);
  queue_families.size = queue_family_count;

  bool present_family_found = false;

  int index_num = 0;
  for (int loop_num = 0; loop_num < queue_family_count; loop_num++) {
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, index_num, vulkan_state->surface, &present_support);

    if (((VkQueueFamilyProperties*)vector_get(&queue_families, loop_num))->queueCount > 0 && present_support) {
      (&vulkan_state->indices)->present_family = index_num;
      present_family_found = true;
    }

    if (present_family_found)
      break;

    index_num++;
  }

  if (!present_family_found)
    return false;

  return true;
  // TODO: Should check for extension support
}

void vulkan_surface_cleanup(struct VulkanState* vulkan_state) {
  vkDestroySurfaceKHR(vulkan_state->instance, vulkan_state->surface, NULL);
}

void recreate_swap_chain(struct VulkanState* vulkan_state) {
  //int width = 0, height = 0;
  //while (width == 0 || height == 0) {
  //  glfwGetFramebufferSize(vulkan_state->glfw_window, &width, &height);
  //  glfwWaitEvents();
  //}
  //
  vkDeviceWaitIdle(vulkan_state->device);

  swap_chain_delete(vulkan_state->swap_chain, vulkan_state);

  //create_swap_chain(vulkan_state, width, height);
  //create_image_views(vulkan_state);
  //create_render_pass(vulkan_state);
  //create_graphics_pipeline(vulkan_state);
  //create_depth_resources(vulkan_state);
  //create_framebuffers(vulkan_state);
  //create_command_buffers(vulkan_state);
}

static VkSampleCountFlagBits vulkan_renderer_get_max_usable_sample_count(struct VulkanState* vulkan_state) {
  return VK_SAMPLE_COUNT_1_BIT;
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
