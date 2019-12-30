#include "mana/graphics/render/swapchain.h"

int swapchain_init(struct SwapChain* swap_chain, struct VulkanRenderer* vulkan_renderer) {
  // TODO: Clean this up like GBuffer
  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
    VkImageViewCreateInfo view_info = {0};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = swap_chain->swap_chain_images[i];
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = vulkan_renderer->swap_chain_image_format;
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
    framebuffer_info.width = vulkan_renderer->swap_chain_extent.width;
    framebuffer_info.height = vulkan_renderer->swap_chain_extent.height;
    framebuffer_info.layers = 1;

    if (vkCreateFramebuffer(vulkan_renderer->device, &framebuffer_info, NULL, &swap_chain->swap_chain_framebuffers[loop_num]) != VK_SUCCESS)
      return VULKAN_RENDERER_CREATE_FRAME_BUFFER_ERROR;
  }

  return 1;
  //return VULKAN_RENDERER_SUCCESS;
}

void swapchain_delete(struct SwapChain* swap_chain, struct VulkanRenderer* vulkan_renderer) {
  vkDestroyRenderPass(vulkan_renderer->device, swap_chain->render_pass, NULL);
}