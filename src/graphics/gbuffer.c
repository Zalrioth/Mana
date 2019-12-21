#include "graphics/gbuffer.h"

int gbuffer_init(struct GBuffer* gbuffer, struct VulkanRenderer* vulkan_renderer) {
  // Image view
  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
    VkImageViewCreateInfo viewInfo = {0};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = gbuffer->swap_chain_images[i];
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = vulkan_renderer->swap_chain_image_format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(vulkan_renderer->device, &viewInfo, NULL, &gbuffer->swap_chain_image_views[i]) != VK_SUCCESS)
      return VULKAN_RENDERER_CREATE_IMAGE_VIEWS_ERROR;
  }

  //VkImageViewCreateInfo viewInfo = {0};
  //viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  //viewInfo.image = gbuffer->normal_image;
  //viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  //viewInfo.format = vulkan_renderer->swap_chain_image_format;
  //viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  //viewInfo.subresourceRange.baseMipLevel = 0;
  //viewInfo.subresourceRange.levelCount = 1;
  //viewInfo.subresourceRange.baseArrayLayer = 0;
  //viewInfo.subresourceRange.layerCount = 1;
  //
  //if (vkCreateImageView(vulkan_renderer->device, &viewInfo, NULL, &gbuffer->normal_image_view) != VK_SUCCESS)
  //  return VULKAN_RENDERER_CREATE_IMAGE_VIEWS_ERROR;

  // Render pass
  struct VkAttachmentDescription color_attachment = {0};
  create_color_attachment(vulkan_renderer, &color_attachment);

  struct VkAttachmentDescription depth_attachment = {0};
  create_depth_attachment(vulkan_renderer, &depth_attachment);

  //struct VkAttachmentDescription normal_attachment = {0};
  //create_color_attachment(vulkan_renderer, &normal_attachment);

  VkAttachmentReference color_attachment_ref = {0};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_attachment_ref = {0};
  depth_attachment_ref.attachment = 1;
  depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  //VkAttachmentReference normal_attachment_ref = {0};
  //normal_attachment_ref.attachment = 2;
  //normal_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  //struct VkAttachmentReference color_attachment_references[2] = {color_attachment_ref, normal_attachment_ref};
  struct VkAttachmentReference color_attachment_references = color_attachment_ref;
  VkSubpassDescription subpass = {0};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  //subpass.colorAttachmentCount = 2;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_references;
  subpass.pDepthStencilAttachment = &depth_attachment_ref;

  VkSubpassDependency dependency = {0};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  //VkAttachmentDescription attachments[3] = {color_attachment, depth_attachment, normal_attachment};
  VkAttachmentDescription attachments[2] = {color_attachment, depth_attachment};
  VkRenderPassCreateInfo render_pass_info = {0};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  //render_pass_info.attachmentCount = 3;
  render_pass_info.attachmentCount = 2;
  render_pass_info.pAttachments = attachments;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;

  if (vkCreateRenderPass(vulkan_renderer->device, &render_pass_info, NULL, &gbuffer->render_pass) != VK_SUCCESS)
    return 0;

  //return VULKAN_RENDERER_CREATE_RENDER_PASS_ERROR;

  /*VkAttachmentDescription color_attachment = {0};
  color_attachment.format = vulkan_renderer->swap_chain_image_format;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentDescription depth_attachment = {0};
  depth_attachment.format = find_depth_format(vulkan_renderer);
  depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference color_attachment_ref = {0};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_attachment_ref = {0};
  depth_attachment_ref.attachment = 1;
  depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {0};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;
  subpass.pDepthStencilAttachment = &depth_attachment_ref;

  VkSubpassDependency dependency = {0};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkAttachmentDescription attachments[2] = {color_attachment, depth_attachment};
  VkRenderPassCreateInfo render_pass_info = {0};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = 2;
  render_pass_info.pAttachments = attachments;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;

  if (vkCreateRenderPass(vulkan_renderer->device, &render_pass_info, NULL, &vulkan_renderer->gbuffer->render_pass) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_RENDER_PASS_ERROR;*/

  return 1;
  //return VULKAN_RENDERER_SUCCESS;
}

void gbuffer_delete(struct GBuffer* gbuffer, struct VulkanRenderer* vulkan_renderer) {
  vkDestroyRenderPass(vulkan_renderer->device, vulkan_renderer->gbuffer->render_pass, NULL);
}