#include "mana/graphics/shaders/atmosphericscattering.h"

int atmospheric_scattering_shader_init(struct AtmosphericScatteringShader* atmospheric_scattering_shader, struct VulkanRenderer* vulkan_renderer) {
  // Set up stuff

  //////////////////////////////////////////////////////////////////////

  atmospheric_scattering_shader->shader = calloc(1, sizeof(struct Shader));

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

  if (vkCreateDescriptorSetLayout(vulkan_renderer->device, &layout_info, NULL, &atmospheric_scattering_shader->shader->descriptor_set_layout) != VK_SUCCESS)
    return 0;

  VkDescriptorPoolSize pool_size = {0};
  pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  pool_size.descriptorCount = 2;

  VkDescriptorPoolCreateInfo pool_info = {0};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.poolSizeCount = 1;
  pool_info.pPoolSizes = &pool_size;
  pool_info.maxSets = 1;

  if (vkCreateDescriptorPool(vulkan_renderer->device, &pool_info, NULL, &atmospheric_scattering_shader->shader->descriptor_pool) != VK_SUCCESS) {
    fprintf(stderr, "failed to create descriptor pool!\n");
    return 0;
  }

  VkPipelineVertexInputStateCreateInfo vertex_input_info = {0};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  VkVertexInputBindingDescription binding_description = mesh_sprite_get_binding_description();
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

  shader_init(atmospheric_scattering_shader->shader, vulkan_renderer, "./assets/shaders/spirv/screenspace.vert.spv", "./assets/shaders/spirv/atmosphericscattering.frag.spv", NULL, vertex_input_info, vulkan_renderer->gbuffer->render_pass, color_blending, VK_FRONT_FACE_CLOCKWISE, VK_FALSE, VK_SAMPLE_COUNT_1_BIT, false);

  //////////////////////////////////////////////////////////////////////
  // Uniform buffers
  VkDeviceSize uniform_buffer_size = sizeof(struct AtmosphericScatteringUniformBufferObject);
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, uniform_buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &atmospheric_scattering_shader->uniform_buffer, &atmospheric_scattering_shader->uniform_buffer_memory);

  VkDeviceSize uniform_buffer_settings_size = sizeof(struct AtmosphericScatteringUniformBufferObjectSettings);
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, uniform_buffer_settings_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &atmospheric_scattering_shader->uniform_buffer_settings, &atmospheric_scattering_shader->uniform_buffer_settings_memory);

  // Descriptor sets
  VkDescriptorSetLayout layout = {0};
  layout = atmospheric_scattering_shader->shader->descriptor_set_layout;

  VkDescriptorSetAllocateInfo alloc_info = {0};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = atmospheric_scattering_shader->shader->descriptor_pool;
  alloc_info.descriptorSetCount = 1;
  alloc_info.pSetLayouts = &layout;

  if (vkAllocateDescriptorSets(vulkan_renderer->device, &alloc_info, &atmospheric_scattering_shader->descriptor_set) != VK_SUCCESS) {
    fprintf(stderr, "failed to allocate descriptor sets!\n");
    return 0;
  }

  VkDescriptorBufferInfo uniform_buffer_info = {0};
  uniform_buffer_info.buffer = atmospheric_scattering_shader->uniform_buffer;
  uniform_buffer_info.offset = 0;
  uniform_buffer_info.range = sizeof(struct AtmosphericScatteringUniformBufferObject);

  VkDescriptorBufferInfo uniform_buffer_setting_info = {0};
  uniform_buffer_setting_info.buffer = atmospheric_scattering_shader->uniform_buffer_settings;
  uniform_buffer_setting_info.offset = 0;
  uniform_buffer_setting_info.range = sizeof(struct AtmosphericScatteringUniformBufferObjectSettings);

  VkWriteDescriptorSet dcs[2];
  memset(dcs, 0, sizeof(dcs));

  dcs[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  dcs[0].dstSet = atmospheric_scattering_shader->descriptor_set;
  dcs[0].dstBinding = 0;
  dcs[0].dstArrayElement = 0;
  dcs[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  dcs[0].descriptorCount = 1;
  dcs[0].pBufferInfo = &uniform_buffer_info;

  dcs[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  dcs[1].dstSet = atmospheric_scattering_shader->descriptor_set;
  dcs[1].dstBinding = 1;
  dcs[1].dstArrayElement = 0;
  dcs[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  dcs[1].descriptorCount = 1;
  dcs[1].pBufferInfo = &uniform_buffer_setting_info;

  vkUpdateDescriptorSets(vulkan_renderer->device, 2, dcs, 0, NULL);

  atmospheric_scattering_shader->fullscreen_quad = calloc(1, sizeof(struct FullscreenQuad));
  fullscreen_quad_init(atmospheric_scattering_shader->fullscreen_quad, vulkan_renderer);

  return 0;
}

void atmospheric_scattering_shader_delete(struct AtmosphericScatteringShader* atmospheric_scattering_shader, struct VulkanRenderer* vulkan_renderer) {
  vkDestroyBuffer(vulkan_renderer->device, atmospheric_scattering_shader->uniform_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, atmospheric_scattering_shader->uniform_buffer_memory, NULL);

  vkDestroyBuffer(vulkan_renderer->device, atmospheric_scattering_shader->uniform_buffer_settings, NULL);
  vkFreeMemory(vulkan_renderer->device, atmospheric_scattering_shader->uniform_buffer_settings_memory, NULL);

  fullscreen_quad_delete(atmospheric_scattering_shader->fullscreen_quad, vulkan_renderer);
  free(atmospheric_scattering_shader->fullscreen_quad);

  vkDestroyDescriptorPool(vulkan_renderer->device, atmospheric_scattering_shader->shader->descriptor_pool, NULL);

  shader_delete(atmospheric_scattering_shader->shader, vulkan_renderer);
  free(atmospheric_scattering_shader->shader);
}

void atmospheric_scattering_shader_render(struct AtmosphericScatteringShader* atmospheric_scattering_shader, struct VulkanRenderer* vulkan_renderer) {
  //struct AtmosphericScatteringUniformBufferObject ubo = {{{0}}};
  //
  //glm_mat4_copy(vulkan_renderer->gbuffer->projection_matrix, ubo.proj);
  //glm_mat4_copy(vulkan_renderer->gbuffer->view_matrix, ubo.view);
  //glm_mat4_identity(ubo.model);
  //
  //glm_rotate(ubo.model, time / 4, (vec3){0.0f, 0.0f + entity_num / 3.14159265358979, 1.0f});
  //glm_translate(ubo.model, (vec3){0.0f + entity_num / 3.14159265358979, 0.0f + entity_num / 3.14159265358979, 0.0f + entity_num / 3.14159265358979});
  //ubo.proj[1][1] *= -1;
  //
  //void* data;
  //vkMapMemory(vulkan_renderer->device, ((struct Sprite*)array_list_get(&game->sprites, entity_num))->uniform_buffers_memory, 0, sizeof(struct AtmosphericScatteringUniformBufferObject), 0, &data);
  //memcpy(data, &ubo, sizeof(struct UniformBufferObject));
  //vkUnmapMemory(vulkan_renderer->device, ((struct Sprite*)array_list_get(&game->sprites, entity_num))->uniform_buffers_memory);
  //
  ////////////////////////////////////////////////////////////////////////////////////
  //
  //post_process_start(vulkan_renderer->post_process, vulkan_renderer);
  //
  //vkCmdBindPipeline(vulkan_renderer->post_process->post_process_command_buffers[vulkan_renderer->post_process->ping_pong], VK_PIPELINE_BIND_POINT_GRAPHICS, atmospheric_scattering_shader->shader->graphics_pipeline);
  //VkBuffer vertex_buffers[] = {atmospheric_scattering_shader->fullscreen_quad->vertex_buffer};
  //VkDeviceSize offsets[] = {0};
  //vkCmdBindVertexBuffers(vulkan_renderer->post_process->post_process_command_buffers[vulkan_renderer->post_process->ping_pong], 0, 1, vertex_buffers, offsets);
  //vkCmdBindIndexBuffer(vulkan_renderer->post_process->post_process_command_buffers[vulkan_renderer->post_process->ping_pong], atmospheric_scattering_shader->fullscreen_quad->index_buffer, 0, VK_INDEX_TYPE_UINT32);
  //vkCmdBindDescriptorSets(vulkan_renderer->post_process->post_process_command_buffers[vulkan_renderer->post_process->ping_pong], VK_PIPELINE_BIND_POINT_GRAPHICS, atmospheric_scattering_shader->shader->pipeline_layout, 0, 1, &atmospheric_scattering_shader->descriptor_sets[vulkan_renderer->post_process->ping_pong ^ true], 0, NULL);
  //vkCmdDrawIndexed(vulkan_renderer->post_process->post_process_command_buffers[vulkan_renderer->post_process->ping_pong], atmospheric_scattering_shader->fullscreen_quad->mesh->indices->size, 1, 0, 0, 0);
  //
  //post_process_stop(vulkan_renderer->post_process, vulkan_renderer);
}
