#include "mana/graphics/shaders/fxaashader.h"

#include "mana/graphics/shaders/spriteshader.h"

int fxaa_shader_init(struct FXAAShader* fxaa_shader, struct VulkanRenderer* vulkan_renderer) {
  fxaa_shader->shader = calloc(1, sizeof(struct Shader));

  VkDescriptorSetLayoutBinding sampler_layout_binding = {0};
  sampler_layout_binding.binding = 0;
  sampler_layout_binding.descriptorCount = 1;
  sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  sampler_layout_binding.pImmutableSamplers = NULL;
  sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutCreateInfo layout_info = {0};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = 1;
  layout_info.pBindings = &sampler_layout_binding;

  if (vkCreateDescriptorSetLayout(vulkan_renderer->device, &layout_info, NULL, &fxaa_shader->shader->descriptor_set_layout) != VK_SUCCESS)
    return 0;

  VkDescriptorPoolSize pool_size;
  memset(&pool_size, 0, sizeof(pool_size));

  pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  pool_size.descriptorCount = 2;

  VkDescriptorPoolCreateInfo pool_info = {0};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.poolSizeCount = 1;
  pool_info.pPoolSizes = &pool_size;
  pool_info.maxSets = 2;

  if (vkCreateDescriptorPool(vulkan_renderer->device, &pool_info, NULL, &fxaa_shader->shader->descriptor_pool) != VK_SUCCESS) {
    fprintf(stderr, "failed to create descriptor pool!\n");
    return 0;
  }

  VkPipelineVertexInputStateCreateInfo vertex_input_info = {0};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  VkVertexInputBindingDescription binding_description = get_binding_description();
  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.vertexAttributeDescriptionCount = 0;  // Note: length of attributeDescriptions
  vertex_input_info.pVertexBindingDescriptions = &binding_description;
  vertex_input_info.pVertexAttributeDescriptions = NULL;

  VkPipelineColorBlendAttachmentState color_blend_attachment = {0};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo color_blending = {0};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY;
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;
  color_blending.blendConstants[0] = 0.0f;
  color_blending.blendConstants[1] = 0.0f;
  color_blending.blendConstants[2] = 0.0f;
  color_blending.blendConstants[3] = 0.0f;

  shader_init(fxaa_shader->shader, vulkan_renderer, "./assets/shaders/spirv/screenspace.vert.spv", "./assets/shaders/spirv/fxaa.frag.spv", NULL, vertex_input_info, vulkan_renderer->post_process->render_pass, color_blending, VK_FALSE, VK_SAMPLE_COUNT_1_BIT);

  //////////////////////////////////////////////////////////////////////

  VkDescriptorSetLayout layout = {0};
  layout = fxaa_shader->shader->descriptor_set_layout;

  VkDescriptorSetAllocateInfo alloc_info = {0};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = fxaa_shader->shader->descriptor_pool;
  alloc_info.descriptorSetCount = 1;
  alloc_info.pSetLayouts = &layout;

  memset(fxaa_shader->descriptor_sets, 0, sizeof(fxaa_shader->descriptor_sets));

  for (int ping_pong_target = 0; ping_pong_target <= 1; ping_pong_target++) {
    if (vkAllocateDescriptorSets(vulkan_renderer->device, &alloc_info, &fxaa_shader->descriptor_sets[ping_pong_target]) != VK_SUCCESS) {
      fprintf(stderr, "failed to allocate descriptor sets!\n");
      return 0;
    }

    VkDescriptorImageInfo image_info = {0};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = vulkan_renderer->post_process->color_image_views[ping_pong_target];
    image_info.sampler = vulkan_renderer->post_process->texture_sampler;

    VkWriteDescriptorSet dc;
    memset(&dc, 0, sizeof(dc));

    dc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    dc.dstSet = fxaa_shader->descriptor_sets[ping_pong_target];
    dc.dstBinding = 0;
    dc.dstArrayElement = 0;
    dc.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    dc.descriptorCount = 1;
    dc.pImageInfo = &image_info;

    vkUpdateDescriptorSets(vulkan_renderer->device, 1, &dc, 0, NULL);
  }

  fxaa_shader->fullscreen_quad = calloc(1, sizeof(struct FullscreenQuad));
  fullscreen_quad_init(fxaa_shader->fullscreen_quad, vulkan_renderer);

  return 0;
}

void fxaa_shader_delete(struct FXAAShader* fxaa_shader, struct VulkanRenderer* vulkan_renderer) {
  fullscreen_quad_delete(fxaa_shader->fullscreen_quad, vulkan_renderer);
  free(fxaa_shader->fullscreen_quad);
  vkDestroyDescriptorPool(vulkan_renderer->device, fxaa_shader->shader->descriptor_pool, NULL);
  shader_delete(fxaa_shader->shader, vulkan_renderer);
  free(fxaa_shader->shader);
}

void fxaa_shader_render(struct FXAAShader* fxaa_shader, struct VulkanRenderer* vulkan_renderer) {
  post_process_start(vulkan_renderer->post_process, vulkan_renderer);

  vkCmdBindPipeline(vulkan_renderer->post_process->post_process_command_buffers[vulkan_renderer->post_process->ping_pong], VK_PIPELINE_BIND_POINT_GRAPHICS, fxaa_shader->shader->graphics_pipeline);
  VkBuffer vertex_buffers[] = {fxaa_shader->fullscreen_quad->vertex_buffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(vulkan_renderer->post_process->post_process_command_buffers[vulkan_renderer->post_process->ping_pong], 0, 1, vertex_buffers, offsets);
  vkCmdBindIndexBuffer(vulkan_renderer->post_process->post_process_command_buffers[vulkan_renderer->post_process->ping_pong], fxaa_shader->fullscreen_quad->index_buffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdBindDescriptorSets(vulkan_renderer->post_process->post_process_command_buffers[vulkan_renderer->post_process->ping_pong], VK_PIPELINE_BIND_POINT_GRAPHICS, fxaa_shader->shader->pipeline_layout, 0, 1, &fxaa_shader->descriptor_sets[vulkan_renderer->post_process->ping_pong ^ true], 0, NULL);
  vkCmdDrawIndexed(vulkan_renderer->post_process->post_process_command_buffers[vulkan_renderer->post_process->ping_pong], fxaa_shader->fullscreen_quad->mesh->indices->size, 1, 0, 0, 0);

  post_process_stop(vulkan_renderer->post_process, vulkan_renderer);
}
