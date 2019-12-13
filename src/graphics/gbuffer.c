#include "graphics/gbuffer.h"

int gbuffer_init(struct GBuffer* gbuffer, struct VulkanRenderer* vulkan_renderer) {
  struct VkAttachmentDescription color_attachment = {0};
  create_color_attachment(vulkan_renderer, &color_attachment);

  struct VkAttachmentDescription depth_attachment = {0};
  create_depth_attachment(vulkan_renderer, &depth_attachment);

  struct VkAttachmentDescription normal_attachment = {0};
  create_color_attachment(vulkan_renderer, &normal_attachment);

  VkAttachmentReference color_attachment_ref = {0};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_attachment_ref = {0};
  depth_attachment_ref.attachment = 1;
  depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference normal_attachment_ref = {0};
  normal_attachment_ref.attachment = 2;
  normal_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

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

  VkAttachmentDescription attachments[3] = {color_attachment, depth_attachment, normal_attachment};
  VkRenderPassCreateInfo render_pass_info = {0};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = 3;
  render_pass_info.pAttachments = attachments;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;

  if (vkCreateRenderPass(vulkan_renderer->device, &render_pass_info, NULL, &vulkan_renderer->render_pass) != VK_SUCCESS)
    return 0;
  //return VULKAN_RENDERER_CREATE_RENDER_PASS_ERROR;

  create_depth_resources(vulkan_renderer, gbuffer->depth_image, gbuffer->depth_image_memory, gbuffer->depth_image_view);

  return 1;
  //return VULKAN_RENDERER_SUCCESS;
}