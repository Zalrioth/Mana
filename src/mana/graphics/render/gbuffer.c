#include "mana/graphics/render/gbuffer.h"

int gbuffer_init(struct GBuffer* gbuffer, struct VulkanRenderer* vulkan_renderer) {
  // NOTE: Look into why format is required to match swap chain
  //VkMemoryPropertyFlags properties =
  graphics_utils_create_image(vulkan_renderer->device, vulkan_renderer->physical_device, vulkan_renderer->swap_chain_extent.width, vulkan_renderer->swap_chain_extent.height, VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &gbuffer->color_image, &gbuffer->color_image_memory);
  graphics_utils_create_image_view(vulkan_renderer->device, gbuffer->color_image, VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_IMAGE_ASPECT_COLOR_BIT, &vulkan_renderer->gbuffer->color_image_view);
  //graphics_utils_transition_image_layout(vulkan_renderer->device, vulkan_renderer->graphics_queue, vulkan_renderer->command_pool, &gbuffer->color_image, VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  graphics_utils_create_image(vulkan_renderer->device, vulkan_renderer->physical_device, vulkan_renderer->swap_chain_extent.width, vulkan_renderer->swap_chain_extent.height, VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &gbuffer->normal_image, &gbuffer->normal_image_memory);
  graphics_utils_create_image_view(vulkan_renderer->device, gbuffer->normal_image, VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_IMAGE_ASPECT_COLOR_BIT, &vulkan_renderer->gbuffer->normal_image_view);
  //graphics_utils_transition_image_layout(vulkan_renderer->device, vulkan_renderer->graphics_queue, vulkan_renderer->command_pool, &gbuffer->normal_image, VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  VkFormat depth_format = find_depth_format(vulkan_renderer);
  graphics_utils_create_image(vulkan_renderer->device, vulkan_renderer->physical_device, vulkan_renderer->swap_chain_extent.width, vulkan_renderer->swap_chain_extent.height, depth_format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &gbuffer->depth_image, &gbuffer->depth_image_memory);
  graphics_utils_create_image_view(vulkan_renderer->device, gbuffer->depth_image, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT, &vulkan_renderer->gbuffer->depth_image_view);
  //graphics_utils_transition_image_layout(vulkan_renderer->device, vulkan_renderer->graphics_queue, vulkan_renderer->command_pool, &gbuffer->depth_image, depth_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

  struct VkAttachmentDescription color_attachment = {0};
  create_color_attachment(vulkan_renderer, &color_attachment);

  struct VkAttachmentDescription normal_attachment = {0};
  create_color_attachment(vulkan_renderer, &normal_attachment);

  struct VkAttachmentDescription depth_attachment = {0};
  create_depth_attachment(vulkan_renderer, &depth_attachment);

  VkAttachmentReference color_attachment_ref = {0};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference normal_attachment_ref = {0};
  normal_attachment_ref.attachment = 1;
  normal_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_attachment_ref = {0};
  depth_attachment_ref.attachment = 2;
  depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  struct VkAttachmentReference color_attachment_references[2] = {color_attachment_ref, normal_attachment_ref};
  VkSubpassDescription subpass = {0};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 2;
  subpass.pColorAttachments = color_attachment_references;
  subpass.pDepthStencilAttachment = &depth_attachment_ref;

  VkSubpassDependency dependency = {0};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkAttachmentDescription attachments_render_pass[3] = {color_attachment, normal_attachment, depth_attachment};
  VkRenderPassCreateInfo render_pass_info = {0};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = 3;
  render_pass_info.pAttachments = attachments_render_pass;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;

  if (vkCreateRenderPass(vulkan_renderer->device, &render_pass_info, NULL, &gbuffer->render_pass) != VK_SUCCESS)
    return 0;

  VkImageView attachments_framebuffer[] = {gbuffer->color_image_view, gbuffer->normal_image_view, gbuffer->depth_image_view};
  VkFramebufferCreateInfo framebuffer_info = {0};
  framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebuffer_info.renderPass = gbuffer->render_pass;
  framebuffer_info.attachmentCount = 3;
  framebuffer_info.pAttachments = attachments_framebuffer;
  framebuffer_info.width = vulkan_renderer->swap_chain_extent.width;
  framebuffer_info.height = vulkan_renderer->swap_chain_extent.height;
  framebuffer_info.layers = 1;

  if (vkCreateFramebuffer(vulkan_renderer->device, &framebuffer_info, NULL, &gbuffer->gbuffer_framebuffer) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_FRAME_BUFFER_ERROR;

  graphics_utils_create_sampler(vulkan_renderer->device, &gbuffer->texture_sampler);

  return 1;
  //return VULKAN_RENDERER_SUCCESS;
}

void gbuffer_delete(struct GBuffer* gbuffer, struct VulkanRenderer* vulkan_renderer) {
  vkDestroySampler(vulkan_renderer->device, gbuffer->texture_sampler, NULL);
  vkDestroyFramebuffer(vulkan_renderer->device, gbuffer->gbuffer_framebuffer, NULL);
  vkDestroyRenderPass(vulkan_renderer->device, gbuffer->render_pass, NULL);

  vkDestroyImageView(vulkan_renderer->device, gbuffer->color_image_view, NULL);
  vkDestroyImage(vulkan_renderer->device, gbuffer->color_image, NULL);
  vkFreeMemory(vulkan_renderer->device, gbuffer->color_image_memory, NULL);

  vkDestroyImageView(vulkan_renderer->device, gbuffer->normal_image_view, NULL);
  vkDestroyImage(vulkan_renderer->device, gbuffer->normal_image, NULL);
  vkFreeMemory(vulkan_renderer->device, gbuffer->normal_image_memory, NULL);

  vkDestroyImageView(vulkan_renderer->device, gbuffer->depth_image_view, NULL);
  vkDestroyImage(vulkan_renderer->device, gbuffer->depth_image, NULL);
  vkFreeMemory(vulkan_renderer->device, gbuffer->depth_image_memory, NULL);
}
