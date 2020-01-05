#include "mana/graphics/render/postprocess.h"

int post_process_init(struct PostProcess* post_process, struct VulkanRenderer* vulkan_renderer) {
  post_process->ping_pong = false;

  for (int ping_pong_target = 0; ping_pong_target <= 1; ping_pong_target++) {
    enum VkFormat image_format = VK_FORMAT_R16G16B16A16_SFLOAT;
    graphics_utils_create_image(vulkan_renderer->device, vulkan_renderer->physical_device, vulkan_renderer->swap_chain_extent.width, vulkan_renderer->swap_chain_extent.height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &post_process->color_images[ping_pong_target], &post_process->color_image_memories[ping_pong_target]);
    graphics_utils_create_image_view(vulkan_renderer->device, post_process->color_images[ping_pong_target], VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, &post_process->color_image_views[ping_pong_target]);

    struct VkAttachmentDescription color_attachment = {0};
    create_color_attachment(vulkan_renderer, &color_attachment);
    color_attachment.format = image_format;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference color_attachment_ref = {0};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    struct VkAttachmentReference color_attachment_reference = color_attachment_ref;
    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_reference;
    subpass.pDepthStencilAttachment = VK_NULL_HANDLE;

    const int total_dependencies = 2;
    VkSubpassDependency dependencies[total_dependencies];

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

    VkAttachmentDescription attachments_render_pass = color_attachment;
    VkRenderPassCreateInfo render_pass_info = {0};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.pAttachments = &attachments_render_pass;
    render_pass_info.attachmentCount = 1;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = total_dependencies;
    render_pass_info.pDependencies = dependencies;

    if (vkCreateRenderPass(vulkan_renderer->device, &render_pass_info, NULL, &post_process->render_passes[ping_pong_target]) != VK_SUCCESS)
      return 0;

    VkImageView attachments_framebuffer = post_process->color_image_views[ping_pong_target];
    VkFramebufferCreateInfo framebuffer_info = {0};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = post_process->render_passes[ping_pong_target];
    framebuffer_info.attachmentCount = 1;
    framebuffer_info.pAttachments = &attachments_framebuffer;
    framebuffer_info.width = vulkan_renderer->swap_chain_extent.width;
    framebuffer_info.height = vulkan_renderer->swap_chain_extent.height;
    framebuffer_info.layers = 1;

    if (vkCreateFramebuffer(vulkan_renderer->device, &framebuffer_info, NULL, &post_process->post_process_framebuffers[ping_pong_target]) != VK_SUCCESS)
      return VULKAN_RENDERER_CREATE_FRAME_BUFFER_ERROR;

    VkSemaphoreCreateInfo semaphore_info = {0};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    vkCreateSemaphore(vulkan_renderer->device, &semaphore_info, NULL, &post_process->post_process_semaphores[ping_pong_target]);
  }

  graphics_utils_create_sampler(vulkan_renderer->device, &post_process->texture_sampler);

  return 1;
}

int post_process_delete(struct PostProcess* post_process, struct VulkanRenderer* vulkan_renderer) {
  vkDestroySampler(vulkan_renderer->device, post_process->texture_sampler, NULL);

  for (int ping_pong_target = 0; ping_pong_target <= 1; ping_pong_target++) {
    vkDestroySemaphore(vulkan_renderer->device, post_process->post_process_semaphores[ping_pong_target], NULL);
    vkDestroyFramebuffer(vulkan_renderer->device, post_process->post_process_framebuffers[ping_pong_target], NULL);
    vkDestroyRenderPass(vulkan_renderer->device, post_process->render_passes[ping_pong_target], NULL);

    vkDestroyImageView(vulkan_renderer->device, post_process->color_image_views[ping_pong_target], NULL);
    vkDestroyImage(vulkan_renderer->device, post_process->color_images[ping_pong_target], NULL);
    vkFreeMemory(vulkan_renderer->device, post_process->color_image_memories[ping_pong_target], NULL);
  }

  return 1;
}
