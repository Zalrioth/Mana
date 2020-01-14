#include "mana/graphics/render/postprocess.h"

int post_process_init(struct PostProcess* post_process, struct VulkanRenderer* vulkan_renderer) {
  post_process->ping_pong = false;

  enum VkFormat image_format = VK_FORMAT_R16G16B16A16_SFLOAT;
  struct VkAttachmentDescription color_attachment = {0};
  create_color_attachment(vulkan_renderer, &color_attachment);
  color_attachment.format = image_format;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  VkAttachmentReference color_attachment_ref = {0};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

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

  if (vkCreateRenderPass(vulkan_renderer->device, &render_pass_info, NULL, &post_process->render_pass) != VK_SUCCESS)
    return 0;

  for (int ping_pong_target = 0; ping_pong_target <= 1; ping_pong_target++) {
    graphics_utils_create_image(vulkan_renderer->device, vulkan_renderer->physical_device, vulkan_renderer->swap_chain->swap_chain_extent.width, vulkan_renderer->swap_chain->swap_chain_extent.height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &post_process->color_images[ping_pong_target], &post_process->color_image_memories[ping_pong_target]);
    graphics_utils_create_image_view(vulkan_renderer->device, post_process->color_images[ping_pong_target], VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, &post_process->color_image_views[ping_pong_target]);

    VkImageView attachments_framebuffer = post_process->color_image_views[ping_pong_target];
    VkFramebufferCreateInfo framebuffer_info = {0};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = post_process->render_pass;
    framebuffer_info.attachmentCount = 1;
    framebuffer_info.pAttachments = &attachments_framebuffer;
    framebuffer_info.width = vulkan_renderer->swap_chain->swap_chain_extent.width;
    framebuffer_info.height = vulkan_renderer->swap_chain->swap_chain_extent.height;
    framebuffer_info.layers = 1;

    if (vkCreateFramebuffer(vulkan_renderer->device, &framebuffer_info, NULL, &post_process->post_process_framebuffers[ping_pong_target]) != VK_SUCCESS)
      return VULKAN_RENDERER_CREATE_FRAME_BUFFER_ERROR;

    VkSemaphoreCreateInfo semaphore_info = {0};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    vkCreateSemaphore(vulkan_renderer->device, &semaphore_info, NULL, &post_process->post_process_semaphores[ping_pong_target]);
  }

  graphics_utils_create_sampler(vulkan_renderer->device, &post_process->texture_sampler);

  // Post process command buffer
  VkCommandBufferAllocateInfo alloc_info_post_process = {0};
  alloc_info_post_process.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info_post_process.commandPool = vulkan_renderer->command_pool;
  alloc_info_post_process.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info_post_process.commandBufferCount = 2;

  if (vkAllocateCommandBuffers(vulkan_renderer->device, &alloc_info_post_process, post_process->post_process_command_buffers) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR;

  post_process->blit_post_process = calloc(1, sizeof(struct BlitPostProcess));

  return 1;
}

int post_process_delete(struct PostProcess* post_process, struct VulkanRenderer* vulkan_renderer) {
  blit_post_process_delete(post_process->blit_post_process, vulkan_renderer);
  free(post_process->blit_post_process);

  vkDestroySampler(vulkan_renderer->device, post_process->texture_sampler, NULL);
  vkDestroyRenderPass(vulkan_renderer->device, post_process->render_pass, NULL);

  for (int ping_pong_target = 0; ping_pong_target <= 1; ping_pong_target++) {
    vkDestroySemaphore(vulkan_renderer->device, post_process->post_process_semaphores[ping_pong_target], NULL);
    vkDestroyFramebuffer(vulkan_renderer->device, post_process->post_process_framebuffers[ping_pong_target], NULL);

    vkDestroyImageView(vulkan_renderer->device, post_process->color_image_views[ping_pong_target], NULL);
    vkDestroyImage(vulkan_renderer->device, post_process->color_images[ping_pong_target], NULL);
    vkFreeMemory(vulkan_renderer->device, post_process->color_image_memories[ping_pong_target], NULL);
  }

  return 1;
}

int post_process_start(struct PostProcess* post_process, struct VulkanRenderer* vulkan_renderer) {
  VkCommandBufferBeginInfo begin_info = {0};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

  if (vkBeginCommandBuffer(post_process->post_process_command_buffers[post_process->ping_pong], &begin_info) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR;

  VkRenderPassBeginInfo render_pass_info = {0};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = post_process->render_pass;
  render_pass_info.framebuffer = post_process->post_process_framebuffers[post_process->ping_pong];
  render_pass_info.renderArea.offset.x = 0;
  render_pass_info.renderArea.offset.y = 0;
  render_pass_info.renderArea.extent = vulkan_renderer->swap_chain->swap_chain_extent;

  VkClearValue clear_value = {0};
  VkClearColorValue clear_color = {{0.0f, 0.0f, 0.0f, 0.0f}};
  clear_value.color = clear_color;

  render_pass_info.clearValueCount = 1;
  render_pass_info.pClearValues = &clear_value;

  vkCmdBeginRenderPass(post_process->post_process_command_buffers[post_process->ping_pong], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

  return VULKAN_RENDERER_SUCCESS;
}

int post_process_stop(struct PostProcess* post_process, struct VulkanRenderer* vulkan_renderer) {
  vkCmdEndRenderPass(post_process->post_process_command_buffers[post_process->ping_pong]);

  if (vkEndCommandBuffer(post_process->post_process_command_buffers[post_process->ping_pong]) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR;

  // Send to GPU for offscreen rendering then wait until finished
  VkSubmitInfo post_process_submit_info = {0};
  post_process_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore wait_semaphore = vulkan_renderer->post_process->post_process_semaphores[vulkan_renderer->post_process->ping_pong ^ true];
  VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  post_process_submit_info.waitSemaphoreCount = 1;
  post_process_submit_info.pWaitSemaphores = &wait_semaphore;
  post_process_submit_info.pWaitDstStageMask = &wait_stage;

  post_process_submit_info.commandBufferCount = 1;
  post_process_submit_info.pCommandBuffers = &post_process->post_process_command_buffers[post_process->ping_pong];
  VkSemaphore post_process_signal_semaphore = post_process->post_process_semaphores[post_process->ping_pong];
  post_process_submit_info.signalSemaphoreCount = 1;
  post_process_submit_info.pSignalSemaphores = &post_process_signal_semaphore;

  VkResult result = vkQueueSubmit(vulkan_renderer->graphics_queue, 1, &post_process_submit_info, VK_NULL_HANDLE);

  if (result != VK_SUCCESS)
    fprintf(stderr, "Error to submit GBuffer!\n");

  post_process->ping_pong ^= true;

  return VULKAN_RENDERER_SUCCESS;
}

/////////////////////////////////////////////////

int blit_post_process_init(struct BlitPostProcess* blit_post_process, struct VulkanRenderer* vulkan_renderer) {
  blit_post_process->blit_shader = calloc(1, sizeof(struct BlitShader));
  blit_shader_init(blit_post_process->blit_shader, vulkan_renderer, vulkan_renderer->post_process->render_pass, 1);

  struct Shader* shader = blit_post_process->blit_shader->shader;

  VkDescriptorSetLayout layout = {0};
  layout = shader->descriptor_set_layout;

  VkDescriptorSetAllocateInfo alloc_info = {0};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = shader->descriptor_pool;
  alloc_info.descriptorSetCount = 1;
  alloc_info.pSetLayouts = &layout;

  if (vkAllocateDescriptorSets(vulkan_renderer->device, &alloc_info, &blit_post_process->descriptor_set) != VK_SUCCESS) {
    fprintf(stderr, "failed to allocate descriptor sets!\n");
    return 0;
  }

  VkDescriptorImageInfo image_info = {0};
  image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  image_info.imageView = vulkan_renderer->gbuffer->color_image_view;
  image_info.sampler = vulkan_renderer->gbuffer->texture_sampler;

  VkWriteDescriptorSet dc;
  memset(&dc, 0, sizeof(dc));

  dc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  dc.dstSet = blit_post_process->descriptor_set;
  dc.dstBinding = 0;
  dc.dstArrayElement = 0;
  dc.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  dc.descriptorCount = 1;
  dc.pImageInfo = &image_info;

  vkUpdateDescriptorSets(vulkan_renderer->device, 1, &dc, 0, NULL);

  blit_post_process->fullscreen_quad = calloc(1, sizeof(struct FullscreenQuad));
  fullscreen_quad_init(blit_post_process->fullscreen_quad, vulkan_renderer);

  return 1;
}

void blit_post_process_delete(struct BlitPostProcess* blit_post_process, struct VulkanRenderer* vulkan_renderer) {
  fullscreen_quad_delete(blit_post_process->fullscreen_quad, vulkan_renderer);
  free(blit_post_process->fullscreen_quad);
  blit_shader_delete(blit_post_process->blit_shader, vulkan_renderer);
  free(blit_post_process->blit_shader);
}

int blit_post_process_render(struct BlitPostProcess* blit_post_process, struct VulkanRenderer* vulkan_renderer) {
  // Custom for intial blitting to post process image
  struct PostProcess* post_process = vulkan_renderer->post_process;

  VkCommandBufferBeginInfo begin_info = {0};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

  if (vkBeginCommandBuffer(post_process->post_process_command_buffers[post_process->ping_pong], &begin_info) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR;

  VkRenderPassBeginInfo render_pass_info = {0};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = post_process->render_pass;
  render_pass_info.framebuffer = post_process->post_process_framebuffers[post_process->ping_pong];
  render_pass_info.renderArea.offset.x = 0;
  render_pass_info.renderArea.offset.y = 0;
  render_pass_info.renderArea.extent = vulkan_renderer->swap_chain->swap_chain_extent;

  VkClearValue clear_value = {0};
  VkClearColorValue clear_color = {{0.0f, 0.0f, 0.0f, 0.0f}};
  clear_value.color = clear_color;

  render_pass_info.clearValueCount = 1;
  render_pass_info.pClearValues = &clear_value;

  vkCmdBeginRenderPass(post_process->post_process_command_buffers[post_process->ping_pong], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

  // The magic
  vkCmdBindPipeline(vulkan_renderer->post_process->post_process_command_buffers[post_process->ping_pong], VK_PIPELINE_BIND_POINT_GRAPHICS, blit_post_process->blit_shader->shader->graphics_pipeline);
  VkBuffer vertex_buffers[] = {blit_post_process->fullscreen_quad->vertex_buffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(vulkan_renderer->post_process->post_process_command_buffers[post_process->ping_pong], 0, 1, vertex_buffers, offsets);
  vkCmdBindIndexBuffer(vulkan_renderer->post_process->post_process_command_buffers[post_process->ping_pong], blit_post_process->fullscreen_quad->index_buffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdBindDescriptorSets(vulkan_renderer->post_process->post_process_command_buffers[post_process->ping_pong], VK_PIPELINE_BIND_POINT_GRAPHICS, blit_post_process->blit_shader->shader->pipeline_layout, 0, 1, &blit_post_process->descriptor_set, 0, NULL);
  // End of the magic
  vkCmdDrawIndexed(vulkan_renderer->post_process->post_process_command_buffers[post_process->ping_pong], blit_post_process->fullscreen_quad->mesh->indices->size, 1, 0, 0, 0);

  vkCmdEndRenderPass(post_process->post_process_command_buffers[post_process->ping_pong]);

  if (vkEndCommandBuffer(post_process->post_process_command_buffers[post_process->ping_pong]) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR;

  // Send to GPU for offscreen rendering then wait until finished
  VkSubmitInfo post_process_submit_info = {0};
  post_process_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore wait_semaphore = vulkan_renderer->gbuffer->gbuffer_semaphore;
  VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  post_process_submit_info.waitSemaphoreCount = 1;
  post_process_submit_info.pWaitSemaphores = &wait_semaphore;
  post_process_submit_info.pWaitDstStageMask = &wait_stage;

  post_process_submit_info.commandBufferCount = 1;
  post_process_submit_info.pCommandBuffers = &post_process->post_process_command_buffers[post_process->ping_pong];
  VkSemaphore post_process_signal_semaphore = post_process->post_process_semaphores[post_process->ping_pong];
  post_process_submit_info.signalSemaphoreCount = 1;
  post_process_submit_info.pSignalSemaphores = &post_process_signal_semaphore;

  VkResult result = vkQueueSubmit(vulkan_renderer->graphics_queue, 1, &post_process_submit_info, VK_NULL_HANDLE);

  if (result != VK_SUCCESS)
    fprintf(stderr, "Error to submit GBuffer!\n");

  post_process->ping_pong ^= true;

  return VULKAN_RENDERER_SUCCESS;
}
