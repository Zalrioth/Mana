#include "mana/graphics/entities/blitpostprocess.h"

int blit_post_process_init(struct BlitPostProcess* blit_post_process, struct VulkanRenderer* vulkan_renderer) {
  blit_shader_init(&blit_post_process->blit_shader, vulkan_renderer, vulkan_renderer->post_process->render_pass, 1);

  struct Shader* shader = &blit_post_process->blit_shader.shader;

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

  // Stuff
  blit_post_process->image_mesh = calloc(1, sizeof(struct Mesh));
  mesh_init(blit_post_process->image_mesh);

  mesh_assign_vertex(blit_post_process->image_mesh->vertices, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  mesh_assign_vertex(blit_post_process->image_mesh->vertices, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
  mesh_assign_vertex(blit_post_process->image_mesh->vertices, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
  mesh_assign_vertex(blit_post_process->image_mesh->vertices, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

  mesh_assign_indice(blit_post_process->image_mesh->indices, 0);
  mesh_assign_indice(blit_post_process->image_mesh->indices, 1);
  mesh_assign_indice(blit_post_process->image_mesh->indices, 2);
  mesh_assign_indice(blit_post_process->image_mesh->indices, 2);
  mesh_assign_indice(blit_post_process->image_mesh->indices, 3);
  mesh_assign_indice(blit_post_process->image_mesh->indices, 0);

  // Vertex
  VkDeviceSize buffer_size_vertex = blit_post_process->image_mesh->vertices->memory_size * blit_post_process->image_mesh->vertices->size;
  //VkDeviceSize bufferSize = sizeof(vulkan_renderer->imageVertices.items[0]) * vulkan_renderer->imageVertices.total;

  VkBuffer staging_buffer_vertex = {0};
  VkDeviceMemory stagingBufferMemory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size_vertex, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer_vertex, &stagingBufferMemory);

  void* data_vertex;
  vkMapMemory(vulkan_renderer->device, stagingBufferMemory, 0, buffer_size_vertex, 0, &data_vertex);
  memcpy(data_vertex, blit_post_process->image_mesh->vertices->items, buffer_size_vertex);
  vkUnmapMemory(vulkan_renderer->device, stagingBufferMemory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size_vertex, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &blit_post_process->vertex_buffer, &blit_post_process->vertex_buffer_memory);

  copy_buffer(vulkan_renderer, staging_buffer_vertex, blit_post_process->vertex_buffer, buffer_size_vertex);

  vkDestroyBuffer(vulkan_renderer->device, staging_buffer_vertex, NULL);
  vkFreeMemory(vulkan_renderer->device, stagingBufferMemory, NULL);

  // Index
  VkDeviceSize buffer_size_index = blit_post_process->image_mesh->indices->memory_size * blit_post_process->image_mesh->indices->size;
  //VkDeviceSize bufferSize = sizeof(vulkan_renderer->imageIndices.items[0]) * vulkan_renderer->imageIndices.total;

  VkBuffer staging_buffer_index = {0};
  VkDeviceMemory staging_buffer_memory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size_index, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer_index, &staging_buffer_memory);

  void* data_index;
  vkMapMemory(vulkan_renderer->device, staging_buffer_memory, 0, buffer_size_index, 0, &data_index);
  memcpy(data_index, blit_post_process->image_mesh->indices->items, buffer_size_index);
  vkUnmapMemory(vulkan_renderer->device, staging_buffer_memory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size_index, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &blit_post_process->index_buffer, &blit_post_process->index_buffer_memory);

  copy_buffer(vulkan_renderer, staging_buffer_index, blit_post_process->index_buffer, buffer_size_index);

  vkDestroyBuffer(vulkan_renderer->device, staging_buffer_index, NULL);
  vkFreeMemory(vulkan_renderer->device, staging_buffer_memory, NULL);

  return 1;
}

void blit_post_process_delete(struct BlitPostProcess* blit_post_process, struct VulkanRenderer* vulkan_renderer) {
  vkDestroyBuffer(vulkan_renderer->device, blit_post_process->index_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, blit_post_process->index_buffer_memory, NULL);

  vkDestroyBuffer(vulkan_renderer->device, blit_post_process->vertex_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, blit_post_process->vertex_buffer_memory, NULL);

  mesh_delete(blit_post_process->image_mesh);
  free(blit_post_process->image_mesh);

  blit_shader_delete(&blit_post_process->blit_shader, vulkan_renderer);
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
  vkCmdBindPipeline(vulkan_renderer->post_process->post_process_command_buffers[post_process->ping_pong], VK_PIPELINE_BIND_POINT_GRAPHICS, blit_post_process->blit_shader.shader.graphics_pipeline);
  VkBuffer vertex_buffers[] = {blit_post_process->vertex_buffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(vulkan_renderer->post_process->post_process_command_buffers[post_process->ping_pong], 0, 1, vertex_buffers, offsets);
  vkCmdBindIndexBuffer(vulkan_renderer->post_process->post_process_command_buffers[post_process->ping_pong], blit_post_process->index_buffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdBindDescriptorSets(vulkan_renderer->post_process->post_process_command_buffers[post_process->ping_pong], VK_PIPELINE_BIND_POINT_GRAPHICS, blit_post_process->blit_shader.shader.pipeline_layout, 0, 1, &blit_post_process->descriptor_set, 0, NULL);
  // End of the magic
  vkCmdDrawIndexed(vulkan_renderer->post_process->post_process_command_buffers[post_process->ping_pong], blit_post_process->image_mesh->indices->size, 1, 0, 0, 0);

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