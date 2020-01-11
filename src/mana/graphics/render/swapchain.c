#include "mana/graphics/render/swapchain.h"

int swap_chain_init(struct SwapChain* swap_chain, struct VulkanRenderer* vulkan_renderer, int width, int height) {
  swap_chain->current_frame = 0;
  struct SwapChainSupportDetails swap_chain_support = {{0}};

  vector_init(&swap_chain_support.formats, sizeof(struct VkSurfaceFormatKHR));
  vector_init(&swap_chain_support.present_modes, sizeof(enum VkPresentModeKHR));

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan_renderer->physical_device, vulkan_renderer->surface, &swap_chain_support.capabilities);

  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_renderer->physical_device, vulkan_renderer->surface, &format_count, NULL);

  if (format_count != 0) {
    vector_resize(&swap_chain_support.formats, format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_renderer->physical_device, vulkan_renderer->surface, &format_count, swap_chain_support.formats.items);
    swap_chain_support.formats.size = format_count;
  }

  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan_renderer->physical_device, vulkan_renderer->surface, &present_mode_count, NULL);

  if (present_mode_count != 0) {
    vector_resize(&swap_chain_support.present_modes, present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan_renderer->physical_device, vulkan_renderer->surface, &present_mode_count, swap_chain_support.present_modes.items);
    swap_chain_support.present_modes.size = present_mode_count;
  }

  VkSurfaceFormatKHR surface_format = {0};

  //if (format_count == 1 && swap_chain_support.formats[0].format == VK_FORMAT_UNDEFINED) {
  if (format_count == 1 && ((struct VkSurfaceFormatKHR*)vector_get(&swap_chain_support.formats, 0))->format == VK_FORMAT_UNDEFINED) {
    surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
    surface_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  } else {
    for (int loop_num = 0; loop_num < vector_size(&swap_chain_support.formats); loop_num++) {
      if (((struct VkSurfaceFormatKHR*)vector_get(&swap_chain_support.formats, loop_num))->format == VK_FORMAT_B8G8R8A8_UNORM && ((struct VkSurfaceFormatKHR*)vector_get(&swap_chain_support.formats, loop_num))->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        surface_format = *(struct VkSurfaceFormatKHR*)vector_get(&swap_chain_support.formats, loop_num);
        break;
      } else
        surface_format = *(struct VkSurfaceFormatKHR*)vector_get(&swap_chain_support.formats, 0);
    }
  }

  // Testing 10 bit swapchain output
  // Seems to work on Nvidia Studio Driver
  //surface_format.format = VK_FORMAT_A2B10G10R10_UNORM_PACK32;

  VkPresentModeKHR present_mode = {0};

  for (int loop_num = 0; loop_num < vector_size(&swap_chain_support.present_modes); loop_num++) {
    if ((enum VkPresentModeKHR)vector_get(&swap_chain_support.present_modes, loop_num) == VK_PRESENT_MODE_MAILBOX_KHR) {
      present_mode = (enum VkPresentModeKHR)vector_get(&swap_chain_support.present_modes, loop_num);
      break;
    } else if ((enum VkPresentModeKHR)vector_get(&swap_chain_support.present_modes, loop_num) == VK_PRESENT_MODE_IMMEDIATE_KHR)
      present_mode = (enum VkPresentModeKHR)vector_get(&swap_chain_support.present_modes, loop_num);
  }

  // Force Vsync
  present_mode = VK_PRESENT_MODE_FIFO_KHR;

  VkExtent2D extent = {width, height};
  if (swap_chain_support.capabilities.currentExtent.width != UINT32_MAX)
    extent = swap_chain_support.capabilities.currentExtent;
  else {
    extent.width = MAX(swap_chain_support.capabilities.minImageExtent.width, MIN(swap_chain_support.capabilities.maxImageExtent.width, extent.width));
    extent.height = MAX(swap_chain_support.capabilities.minImageExtent.height, MIN(swap_chain_support.capabilities.maxImageExtent.height, extent.height));
  }

  uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
  if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount)
    image_count = swap_chain_support.capabilities.maxImageCount;

  VkSwapchainCreateInfoKHR swapchain_info = {0};
  swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_info.surface = vulkan_renderer->surface;

  swapchain_info.minImageCount = image_count;
  swapchain_info.imageFormat = surface_format.format;
  swapchain_info.imageColorSpace = surface_format.colorSpace;
  swapchain_info.imageExtent = extent;
  swapchain_info.imageArrayLayers = 1;
  swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  uint32_t queue_family_indices[] = {(&vulkan_renderer->indices)->graphics_family, (&vulkan_renderer->indices)->present_family};

  if ((&vulkan_renderer->indices)->graphics_family != (&vulkan_renderer->indices)->present_family) {
    swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchain_info.queueFamilyIndexCount = 2;
    swapchain_info.pQueueFamilyIndices = queue_family_indices;
  } else
    swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

  swapchain_info.preTransform = swap_chain_support.capabilities.currentTransform;
  swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchain_info.presentMode = present_mode;
  swapchain_info.clipped = VK_TRUE;

  if (vkCreateSwapchainKHR(vulkan_renderer->device, &swapchain_info, NULL, &swap_chain->swap_chain_khr) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_SWAP_CHAIN_ERROR;

  vkGetSwapchainImagesKHR(vulkan_renderer->device, swap_chain->swap_chain_khr, &image_count, NULL);
  vkGetSwapchainImagesKHR(vulkan_renderer->device, swap_chain->swap_chain_khr, &image_count, swap_chain->swap_chain_images);

  swap_chain->swap_chain_image_format = surface_format.format;
  swap_chain->swap_chain_extent = extent;

  vector_delete(&swap_chain_support.formats);
  vector_delete(&swap_chain_support.present_modes);
  ////////////////////////////////////////////////////////

  // TODO: Clean this up like GBuffer
  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
    VkImageViewCreateInfo view_info = {0};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = swap_chain->swap_chain_images[i];
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = vulkan_renderer->swap_chain->swap_chain_image_format;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    if (vkCreateImageView(vulkan_renderer->device, &view_info, NULL, &swap_chain->swap_chain_image_views[i]) != VK_SUCCESS)
      return VULKAN_RENDERER_CREATE_IMAGE_VIEWS_ERROR;
  }

  struct VkAttachmentDescription color_attachment = {0};
  create_color_attachment(vulkan_renderer, &color_attachment);
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

  VkAttachmentReference color_attachment_ref = {0};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  struct VkAttachmentReference color_attachment_references = color_attachment_ref;
  VkSubpassDescription subpass = {0};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_references;

  VkSubpassDependency dependency = {0};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo render_pass_info = {0};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = 1;
  render_pass_info.pAttachments = &color_attachment;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;

  if (vkCreateRenderPass(vulkan_renderer->device, &render_pass_info, NULL, &swap_chain->render_pass) != VK_SUCCESS)
    return 0;

  for (int loop_num = 0; loop_num < MAX_SWAP_CHAIN_FRAMES; loop_num++) {
    VkFramebufferCreateInfo framebuffer_info = {0};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = vulkan_renderer->swap_chain->render_pass;
    framebuffer_info.attachmentCount = 1;
    framebuffer_info.pAttachments = &swap_chain->swap_chain_image_views[loop_num];
    framebuffer_info.width = vulkan_renderer->swap_chain->swap_chain_extent.width;
    framebuffer_info.height = vulkan_renderer->swap_chain->swap_chain_extent.height;
    framebuffer_info.layers = 1;

    if (vkCreateFramebuffer(vulkan_renderer->device, &framebuffer_info, NULL, &swap_chain->swap_chain_framebuffers[loop_num]) != VK_SUCCESS)
      return VULKAN_RENDERER_CREATE_FRAME_BUFFER_ERROR;
  }

  // Swapchain command buffer
  VkCommandBufferAllocateInfo alloc_info_swapchain = {0};
  alloc_info_swapchain.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info_swapchain.commandPool = vulkan_renderer->command_pool;
  alloc_info_swapchain.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info_swapchain.commandBufferCount = (uint32_t)MAX_SWAP_CHAIN_FRAMES;

  if (vkAllocateCommandBuffers(vulkan_renderer->device, &alloc_info_swapchain, swap_chain->swap_chain_command_buffers) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR;

  memset(swap_chain->in_flight_fences, 0, sizeof(swap_chain->in_flight_fences));

  VkSemaphoreCreateInfo semaphore_info = {0};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fence_info = {0};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(vulkan_renderer->device, &semaphore_info, NULL, &swap_chain->image_available_semaphores[i]) != VK_SUCCESS || vkCreateSemaphore(vulkan_renderer->device, &semaphore_info, NULL, &swap_chain->render_finished_semaphores[i]) != VK_SUCCESS || vkCreateFence(vulkan_renderer->device, &fence_info, NULL, &swap_chain->in_flight_fences[i]) != VK_SUCCESS)
      return VULKAN_RENDERER_CREATE_SYNC_OBJECT_ERROR;
  }

  return 1;
  //return VULKAN_RENDERER_SUCCESS;
}

void swap_chain_delete(struct SwapChain* swap_chain, struct VulkanRenderer* vulkan_renderer) {
  for (int loop_num = 0; loop_num < MAX_FRAMES_IN_FLIGHT; loop_num++)
    vkWaitForFences(vulkan_renderer->device, 1, &vulkan_renderer->swap_chain->in_flight_fences[loop_num], VK_TRUE, UINT64_MAX);

  vkDestroyRenderPass(vulkan_renderer->device, swap_chain->render_pass, NULL);

  for (int loop_num = 0; loop_num < MAX_SWAP_CHAIN_FRAMES; loop_num++) {
    vkDestroyFramebuffer(vulkan_renderer->device, swap_chain->swap_chain_framebuffers[loop_num], NULL);
    vkDestroyImageView(vulkan_renderer->device, swap_chain->swap_chain_image_views[loop_num], NULL);
  }

  vkDestroySwapchainKHR(vulkan_renderer->device, vulkan_renderer->swap_chain->swap_chain_khr, NULL);
  vkFreeCommandBuffers(vulkan_renderer->device, vulkan_renderer->command_pool, 3, swap_chain->swap_chain_command_buffers);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(vulkan_renderer->device, swap_chain->render_finished_semaphores[i], NULL);
    vkDestroySemaphore(vulkan_renderer->device, swap_chain->image_available_semaphores[i], NULL);
    vkDestroyFence(vulkan_renderer->device, swap_chain->in_flight_fences[i], NULL);
  }
}

int swap_chain_start(struct SwapChain* swap_chain, struct VulkanRenderer* vulkan_renderer, int swap_chain_num) {
  VkCommandBufferBeginInfo begin_info = {0};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

  if (vkBeginCommandBuffer(swap_chain->swap_chain_command_buffers[swap_chain_num], &begin_info) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR;

  VkRenderPassBeginInfo render_pass_info = {0};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = vulkan_renderer->swap_chain->render_pass;
  render_pass_info.framebuffer = vulkan_renderer->swap_chain->swap_chain_framebuffers[swap_chain_num];
  render_pass_info.renderArea.offset.x = 0;
  render_pass_info.renderArea.offset.y = 0;
  render_pass_info.renderArea.extent = vulkan_renderer->swap_chain->swap_chain_extent;

  VkClearValue clear_value = {0};
  VkClearColorValue clear_color = {{0.0f, 0.0f, 0.0f, 0.0f}};
  clear_value.color = clear_color;

  render_pass_info.clearValueCount = 1;
  render_pass_info.pClearValues = &clear_value;

  vkCmdBeginRenderPass(swap_chain->swap_chain_command_buffers[swap_chain_num], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

  return VULKAN_RENDERER_SUCCESS;
}

int swap_chain_stop(struct SwapChain* swap_chain, struct VulkanRenderer* vulkan_renderer, int swap_chain_num) {
  vkCmdEndRenderPass(swap_chain->swap_chain_command_buffers[swap_chain_num]);

  if (vkEndCommandBuffer(swap_chain->swap_chain_command_buffers[swap_chain_num]) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR;

  return VULKAN_RENDERER_SUCCESS;
}
