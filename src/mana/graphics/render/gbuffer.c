#include "mana/graphics/render/gbuffer.h"

// TODO: Implement sample shading
// https://vulkan-tutorial.com/Multisampling
int gbuffer_init(struct GBuffer* gbuffer, struct VulkanState* vulkan_renderer) {
  VkFormat image_format = VK_FORMAT_R16G16B16A16_SFLOAT;
  VkImageUsageFlags image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  VkFormat depth_format = graphics_utils_find_depth_format(vulkan_renderer->physical_device);

  uint32_t gbuffer_width = vulkan_renderer->swap_chain->swap_chain_extent.width * vulkan_renderer->swap_chain->supersample_scale;
  uint32_t gbuffer_height = vulkan_renderer->swap_chain->swap_chain_extent.height * vulkan_renderer->swap_chain->supersample_scale;

  // Resolve or standard if not multisampling
  graphics_utils_create_image(vulkan_renderer->device, vulkan_renderer->physical_device, gbuffer_width, gbuffer_height, 1, VK_SAMPLE_COUNT_1_BIT, image_format, VK_IMAGE_TILING_OPTIMAL, image_usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &gbuffer->color_image, &gbuffer->color_image_memory);
  graphics_utils_create_image_view(vulkan_renderer->device, gbuffer->color_image, image_format, VK_IMAGE_ASPECT_COLOR_BIT, 1, &vulkan_renderer->gbuffer->color_image_view);

  graphics_utils_create_image(vulkan_renderer->device, vulkan_renderer->physical_device, gbuffer_width, gbuffer_height, 1, VK_SAMPLE_COUNT_1_BIT, image_format, VK_IMAGE_TILING_OPTIMAL, image_usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &gbuffer->normal_image, &gbuffer->normal_image_memory);
  graphics_utils_create_image_view(vulkan_renderer->device, gbuffer->normal_image, image_format, VK_IMAGE_ASPECT_COLOR_BIT, 1, &vulkan_renderer->gbuffer->normal_image_view);

  graphics_utils_create_image(vulkan_renderer->device, vulkan_renderer->physical_device, gbuffer_width, gbuffer_height, 1, vulkan_renderer->msaa_samples, depth_format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &gbuffer->depth_image, &gbuffer->depth_image_memory);
  graphics_utils_create_image_view(vulkan_renderer->device, gbuffer->depth_image, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT, 1, &gbuffer->depth_image_view);

  VkAttachmentDescription color_attachment = {0};
  graphics_utils_create_color_attachment(vulkan_renderer->swap_chain->swap_chain_image_format, &color_attachment);
  color_attachment.format = image_format;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  //color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

  VkAttachmentDescription normal_attachment = {0};
  graphics_utils_create_color_attachment(vulkan_renderer->swap_chain->swap_chain_image_format, &normal_attachment);
  normal_attachment.format = image_format;
  normal_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  VkAttachmentDescription depth_attachment = {0};
  graphics_utils_create_depth_attachment(vulkan_renderer->physical_device, &depth_attachment);
  depth_attachment.samples = vulkan_renderer->msaa_samples;

  VkAttachmentReference color_attachment_ref = {0};
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference normal_attachment_ref = {0};
  normal_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_attachment_ref = {0};
  depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  if (vulkan_renderer->msaa_samples != 1) {
    color_attachment_ref.attachment = 3;
    normal_attachment_ref.attachment = 4;
    depth_attachment_ref.attachment = 2;

    // Multisample
    graphics_utils_create_image(vulkan_renderer->device, vulkan_renderer->physical_device, gbuffer_width, gbuffer_height, 1, vulkan_renderer->msaa_samples, image_format, VK_IMAGE_TILING_OPTIMAL, image_usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &gbuffer->multisample_color_image, &gbuffer->multisample_color_image_memory);
    graphics_utils_create_image_view(vulkan_renderer->device, gbuffer->multisample_color_image, image_format, VK_IMAGE_ASPECT_COLOR_BIT, 1, &vulkan_renderer->gbuffer->multisample_color_image_view);

    graphics_utils_create_image(vulkan_renderer->device, vulkan_renderer->physical_device, gbuffer_width, gbuffer_height, 1, vulkan_renderer->msaa_samples, image_format, VK_IMAGE_TILING_OPTIMAL, image_usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &gbuffer->multisample_normal_image, &gbuffer->multisample_normal_image_memory);
    graphics_utils_create_image_view(vulkan_renderer->device, gbuffer->multisample_normal_image, image_format, VK_IMAGE_ASPECT_COLOR_BIT, 1, &vulkan_renderer->gbuffer->multisample_normal_image_view);

    VkAttachmentDescription multisample_color_attachment = {0};
    graphics_utils_create_color_attachment(vulkan_renderer->swap_chain->swap_chain_image_format, &multisample_color_attachment);
    multisample_color_attachment.format = image_format;
    multisample_color_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    multisample_color_attachment.samples = vulkan_renderer->msaa_samples;
    //multisample_color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

    VkAttachmentDescription multisample_normal_attachment = {0};
    graphics_utils_create_color_attachment(vulkan_renderer->swap_chain->swap_chain_image_format, &multisample_normal_attachment);
    multisample_normal_attachment.format = image_format;
    multisample_normal_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    multisample_normal_attachment.samples = vulkan_renderer->msaa_samples;

    VkAttachmentReference multisample_color_attachment_ref = {0};
    multisample_color_attachment_ref.attachment = 0;
    multisample_color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference multisample_normal_attachment_ref = {0};
    multisample_normal_attachment_ref.attachment = 1;
    multisample_normal_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    ////////////////////////////////////////////////////////////////////////////////////

    VkAttachmentReference multisample_attachment_references[GBUFFER_COLOR_ATTACHMENTS] = {multisample_color_attachment_ref, multisample_normal_attachment_ref};
    VkAttachmentReference color_attachment_references[GBUFFER_COLOR_ATTACHMENTS] = {color_attachment_ref, normal_attachment_ref};
    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = GBUFFER_COLOR_ATTACHMENTS;
    subpass.pColorAttachments = multisample_attachment_references;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;
    subpass.pResolveAttachments = color_attachment_references;

    VkSubpassDependency dependencies[GBUFFER_TOTAL_DEPENDENCIES];
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo render_pass_info = {0};
    VkAttachmentDescription attachments_render_pass[MULTISAMPLE_GBUFFER_TOTAL_ATTACHMENTS] = {multisample_color_attachment, multisample_normal_attachment, depth_attachment, color_attachment, normal_attachment};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.pAttachments = attachments_render_pass;
    render_pass_info.attachmentCount = MULTISAMPLE_GBUFFER_TOTAL_ATTACHMENTS;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = GBUFFER_TOTAL_DEPENDENCIES;
    render_pass_info.pDependencies = dependencies;

    if (vkCreateRenderPass(vulkan_renderer->device, &render_pass_info, NULL, &gbuffer->render_pass) != VK_SUCCESS)
      return 0;

    VkFramebufferCreateInfo framebuffer_info = {0};
    VkImageView attachments_framebuffer[] = {gbuffer->multisample_color_image_view, gbuffer->multisample_normal_image_view, gbuffer->depth_image_view, gbuffer->color_image_view, gbuffer->normal_image_view};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = gbuffer->render_pass;
    framebuffer_info.attachmentCount = MULTISAMPLE_GBUFFER_TOTAL_ATTACHMENTS;
    framebuffer_info.pAttachments = attachments_framebuffer;
    framebuffer_info.width = gbuffer_width;
    framebuffer_info.height = gbuffer_height;
    framebuffer_info.layers = 1;

    if (vkCreateFramebuffer(vulkan_renderer->device, &framebuffer_info, NULL, &gbuffer->gbuffer_framebuffer) != VK_SUCCESS)
      return VULKAN_RENDERER_CREATE_FRAME_BUFFER_ERROR;

    VkSemaphoreCreateInfo semaphore_info = {0};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    vkCreateSemaphore(vulkan_renderer->device, &semaphore_info, NULL, &gbuffer->gbuffer_semaphore);
    graphics_utils_create_sampler(vulkan_renderer->device, &gbuffer->texture_sampler, (struct SamplerSettings){.mip_levels = 0, .filter = VK_FILTER_LINEAR, .address_mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE});

    gbuffer->projection_matrix = MAT4_ZERO;
    gbuffer->view_matrix = MAT4_ZERO;

    // Gbuffer command buffer
    VkCommandBufferAllocateInfo alloc_info_offscreen = {0};
    alloc_info_offscreen.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info_offscreen.commandPool = vulkan_renderer->command_pool;
    alloc_info_offscreen.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info_offscreen.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(vulkan_renderer->device, &alloc_info_offscreen, &gbuffer->gbuffer_command_buffer) != VK_SUCCESS)
      return VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR;
  } else {
    color_attachment_ref.attachment = 0;
    normal_attachment_ref.attachment = 1;
    depth_attachment_ref.attachment = 2;

    struct VkAttachmentReference color_attachment_references[GBUFFER_COLOR_ATTACHMENTS] = {color_attachment_ref, normal_attachment_ref};
    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = GBUFFER_COLOR_ATTACHMENTS;
    subpass.pColorAttachments = color_attachment_references;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    VkSubpassDependency dependencies[GBUFFER_TOTAL_DEPENDENCIES];
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkAttachmentDescription attachments_render_pass[GBUFFER_TOTAL_ATTACHMENTS] = {color_attachment, normal_attachment, depth_attachment};
    VkRenderPassCreateInfo render_pass_info = {0};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.pAttachments = attachments_render_pass;
    render_pass_info.attachmentCount = GBUFFER_TOTAL_ATTACHMENTS;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = GBUFFER_TOTAL_DEPENDENCIES;
    render_pass_info.pDependencies = dependencies;

    if (vkCreateRenderPass(vulkan_renderer->device, &render_pass_info, NULL, &gbuffer->render_pass) != VK_SUCCESS)
      return 0;

    VkFramebufferCreateInfo framebuffer_info = {0};
    VkImageView attachments_framebuffer[] = {gbuffer->color_image_view, gbuffer->normal_image_view, gbuffer->depth_image_view};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = gbuffer->render_pass;
    framebuffer_info.attachmentCount = GBUFFER_TOTAL_ATTACHMENTS;
    framebuffer_info.pAttachments = attachments_framebuffer;
    framebuffer_info.width = gbuffer_width;
    framebuffer_info.height = gbuffer_height;
    framebuffer_info.layers = 1;

    if (vkCreateFramebuffer(vulkan_renderer->device, &framebuffer_info, NULL, &gbuffer->gbuffer_framebuffer) != VK_SUCCESS)
      return VULKAN_RENDERER_CREATE_FRAME_BUFFER_ERROR;

    VkSemaphoreCreateInfo semaphore_info = {0};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    vkCreateSemaphore(vulkan_renderer->device, &semaphore_info, NULL, &gbuffer->gbuffer_semaphore);
    graphics_utils_create_sampler(vulkan_renderer->device, &gbuffer->texture_sampler, (struct SamplerSettings){.mip_levels = 0, .filter = VK_FILTER_LINEAR, .address_mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE});

    gbuffer->projection_matrix = MAT4_ZERO;
    gbuffer->view_matrix = MAT4_ZERO;

    // Gbuffer command buffer
    VkCommandBufferAllocateInfo alloc_info_offscreen = {0};
    alloc_info_offscreen.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info_offscreen.commandPool = vulkan_renderer->command_pool;
    alloc_info_offscreen.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info_offscreen.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(vulkan_renderer->device, &alloc_info_offscreen, &gbuffer->gbuffer_command_buffer) != VK_SUCCESS)
      return VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR;
  }

  return 1;
  //return VULKAN_RENDERER_SUCCESS;
}

void gbuffer_delete(struct GBuffer* gbuffer, struct VulkanState* vulkan_renderer) {
  vkDestroySemaphore(vulkan_renderer->device, gbuffer->gbuffer_semaphore, NULL);
  vkDestroySampler(vulkan_renderer->device, gbuffer->texture_sampler, NULL);
  vkDestroyFramebuffer(vulkan_renderer->device, gbuffer->gbuffer_framebuffer, NULL);
  vkDestroyRenderPass(vulkan_renderer->device, gbuffer->render_pass, NULL);

  // Resolve
  vkDestroyImageView(vulkan_renderer->device, gbuffer->color_image_view, NULL);
  vkDestroyImage(vulkan_renderer->device, gbuffer->color_image, NULL);
  vkFreeMemory(vulkan_renderer->device, gbuffer->color_image_memory, NULL);

  vkDestroyImageView(vulkan_renderer->device, gbuffer->normal_image_view, NULL);
  vkDestroyImage(vulkan_renderer->device, gbuffer->normal_image, NULL);
  vkFreeMemory(vulkan_renderer->device, gbuffer->normal_image_memory, NULL);

  vkDestroyImageView(vulkan_renderer->device, gbuffer->depth_image_view, NULL);
  vkDestroyImage(vulkan_renderer->device, gbuffer->depth_image, NULL);
  vkFreeMemory(vulkan_renderer->device, gbuffer->depth_image_memory, NULL);

  if (vulkan_renderer->msaa_samples != 1) {
    // Multisample
    vkDestroyImageView(vulkan_renderer->device, gbuffer->multisample_color_image_view, NULL);
    vkDestroyImage(vulkan_renderer->device, gbuffer->multisample_color_image, NULL);
    vkFreeMemory(vulkan_renderer->device, gbuffer->multisample_color_image_memory, NULL);

    vkDestroyImageView(vulkan_renderer->device, gbuffer->multisample_normal_image_view, NULL);
    vkDestroyImage(vulkan_renderer->device, gbuffer->multisample_normal_image, NULL);
    vkFreeMemory(vulkan_renderer->device, gbuffer->multisample_normal_image_memory, NULL);
  }
}

int gbuffer_start(struct GBuffer* gbuffer, struct VulkanState* vulkan_renderer) {
  VkCommandBufferBeginInfo begin_info = {0};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

  if (vkBeginCommandBuffer(gbuffer->gbuffer_command_buffer, &begin_info) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR;

  VkRenderPassBeginInfo render_pass_info = {0};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = gbuffer->render_pass;
  render_pass_info.framebuffer = gbuffer->gbuffer_framebuffer;
  render_pass_info.renderArea.offset.x = 0;
  render_pass_info.renderArea.offset.y = 0;
  render_pass_info.renderArea.extent.width = vulkan_renderer->swap_chain->swap_chain_extent.width * vulkan_renderer->swap_chain->supersample_scale;
  render_pass_info.renderArea.extent.height = vulkan_renderer->swap_chain->swap_chain_extent.height * vulkan_renderer->swap_chain->supersample_scale;

  if (vulkan_renderer->msaa_samples != 1) {
    VkClearValue clear_values[MULTISAMPLE_GBUFFER_TOTAL_ATTACHMENTS] = {0};

    // Should not be clearing color only normals and depth
    // Faster to just draw over old color content instead of clearing
    // http://ogldev.atspace.co.uk/www/tutorial51/tutorial51.html
    //VkClearColorValue multisample_clear_color = {{0.0f, 0.0f, 0.0f, 0.0f}};
    VkClearColorValue multisample_clear_color = {{0.529f, 0.808f, 0.922f, 0.0f}};
    clear_values[0].color = multisample_clear_color;

    VkClearColorValue multisample_clear_normals = {{0.0f, 0.0f, 0.0f, 0.0f}};
    clear_values[1].color = multisample_clear_normals;

    VkClearDepthStencilValue clear_depth = {0.0f, 0};
    clear_values[2].depthStencil = clear_depth;

    //VkClearColorValue clear_color = {{0.0f, 0.0f, 0.0f, 0.0f}};
    VkClearColorValue clear_color = {{0.529f, 0.808f, 0.922f, 0.0f}};
    clear_values[3].color = clear_color;

    VkClearColorValue clear_normals = {{0.0f, 0.0f, 0.0f, 0.0f}};
    clear_values[4].color = clear_normals;

    render_pass_info.clearValueCount = MULTISAMPLE_GBUFFER_TOTAL_ATTACHMENTS;
    render_pass_info.pClearValues = clear_values;

    vkCmdBeginRenderPass(gbuffer->gbuffer_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
  } else {
    VkClearValue clear_values[GBUFFER_TOTAL_ATTACHMENTS] = {0};

    //VkClearColorValue clear_color = {{0.0f, 0.0f, 0.0f, 0.0f}};
    VkClearColorValue clear_color = {{0.529f, 0.808f, 0.922f, 0.0f}};
    clear_values[0].color = clear_color;

    VkClearColorValue clear_normals = {{0.0f, 0.0f, 0.0f, 0.0f}};
    clear_values[1].color = clear_normals;

    VkClearDepthStencilValue clear_depth = {0.0f, 0};
    clear_values[2].depthStencil = clear_depth;

    render_pass_info.clearValueCount = GBUFFER_TOTAL_ATTACHMENTS;
    render_pass_info.pClearValues = clear_values;

    vkCmdBeginRenderPass(gbuffer->gbuffer_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
  }

  return VULKAN_RENDERER_SUCCESS;
}

int gbuffer_stop(struct GBuffer* gbuffer, struct VulkanState* vulkan_renderer) {
  vkCmdEndRenderPass(gbuffer->gbuffer_command_buffer);

  if (vkEndCommandBuffer(gbuffer->gbuffer_command_buffer) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR;

  // Send to GPU for offscreen rendering then wait until finished
  VkSubmitInfo gbuffer_submit_info = {0};
  gbuffer_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  gbuffer_submit_info.commandBufferCount = 1;
  gbuffer_submit_info.pCommandBuffers = &gbuffer->gbuffer_command_buffer;
  VkSemaphore signal_semaphores[] = {gbuffer->gbuffer_semaphore};
  gbuffer_submit_info.signalSemaphoreCount = 1;
  gbuffer_submit_info.pSignalSemaphores = signal_semaphores;

  VkResult result = vkQueueSubmit(vulkan_renderer->graphics_queue, 1, &gbuffer_submit_info, VK_NULL_HANDLE);

  if (result != VK_SUCCESS)
    fprintf(stderr, "Error to submit GBuffer!\n");

  return VULKAN_RENDERER_SUCCESS;
}
