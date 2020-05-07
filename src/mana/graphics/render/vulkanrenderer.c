#include "mana/graphics/render/vulkanrenderer.h"

bool vulkan_renderer_can_device_present(struct VulkanState* vulkan_state, VkPhysicalDevice device) {
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

int vulkan_renderer_init(struct VulkanState* vulkan_state, int width, int height) {
  vulkan_state->swap_chain = malloc(sizeof(struct SwapChain));
  vulkan_state->gbuffer = malloc(sizeof(struct GBuffer));
  vulkan_state->post_process = malloc(sizeof(struct PostProcess));
  vulkan_state->msaa_samples = vulkan_core_get_max_usable_sample_count(vulkan_state);

  vulkan_renderer_can_device_present(vulkan_state, vulkan_state->physical_device);

  swap_chain_init(vulkan_state->swap_chain, vulkan_state, width, height);
  post_process_init(vulkan_state->post_process, vulkan_state);
  gbuffer_init(vulkan_state->gbuffer, vulkan_state);

  // Maybe move this
  blit_swap_chain_init(vulkan_state->swap_chain->blit_swap_chain, vulkan_state);
  blit_post_process_init(vulkan_state->post_process->blit_post_process, vulkan_state);

  return VULKAN_RENDERER_SUCCESS;
}

void copy_buffer(struct VulkanState* vulkan_state, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) {
  VkCommandBuffer command_buffer = graphics_utils_begin_single_time_commands(vulkan_state->device, vulkan_state->command_pool);

  VkBufferCopy copy_region = {0};
  copy_region.size = size;
  vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

  graphics_utils_end_single_time_commands(vulkan_state->device, vulkan_state->graphics_queue, vulkan_state->command_pool, command_buffer);
}

void vulkan_device_cleanup(struct VulkanState* vulkan_state) {
  vkDestroyDevice(vulkan_state->device, NULL);
}

void vulkan_surface_cleanup(struct VulkanState* vulkan_state) {
  vkDestroySurfaceKHR(vulkan_state->instance, vulkan_state->surface, NULL);
}

void vulkan_debug_cleanup(struct VulkanState* vulkan_state) {
  //if (enable_validation_layers)
  //    vulkan_core_destroy_debug_utils_messenger_ext(vulkan_state->instance, vulkan_state->debug_messenger, NULL);
}

int create_image_view(struct VulkanState* vulkan_state, struct VkImage_T* image, struct VkImageView_T* image_view) {
  VkImageViewCreateInfo view_info = {0};
  view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_info.image = image;
  view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  view_info.format = vulkan_state->swap_chain->swap_chain_image_format;
  view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  view_info.subresourceRange.baseMipLevel = 0;
  view_info.subresourceRange.levelCount = 1;
  view_info.subresourceRange.baseArrayLayer = 0;
  view_info.subresourceRange.layerCount = 1;

  if (vkCreateImageView(vulkan_state->device, &view_info, NULL, &image_view) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_IMAGE_VIEWS_ERROR;

  return VULKAN_RENDERER_SUCCESS;
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
