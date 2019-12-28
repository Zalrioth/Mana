#include "mana/graphics/effects/bliteffect.h"

int blit_effect_init(struct BlitEffect* blit_effect, struct VulkanRenderer* vulkan_renderer) {
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

  if (vkCreateDescriptorSetLayout(vulkan_renderer->device, &layout_info, NULL, &blit_effect->shader.descriptor_set_layout) != VK_SUCCESS)
    return 0;

  VkDescriptorPoolSize pool_size;
  memset(&pool_size, 0, sizeof(pool_size));

  // Post process and GUI per swap chain image?
  int sprite_descriptors = 1;
  pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  pool_size.descriptorCount = sprite_descriptors;

  VkDescriptorPoolCreateInfo poolInfo = {0};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 1;
  poolInfo.pPoolSizes = &pool_size;
  poolInfo.maxSets = sprite_descriptors;

  if (vkCreateDescriptorPool(vulkan_renderer->device, &poolInfo, NULL, &blit_effect->shader.descriptor_pool) != VK_SUCCESS) {
    fprintf(stderr, "failed to create descriptor pool!\n");
    return 0;
  }

  VkPipelineVertexInputStateCreateInfo vertex_input_info = {0};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  VkVertexInputBindingDescription binding_description = get_binding_description();
  vertex_input_info.vertexBindingDescriptionCount = 0;
  vertex_input_info.vertexAttributeDescriptionCount = 0;  // Note: length of attributeDescriptions
  vertex_input_info.pVertexBindingDescriptions = &binding_description;
  vertex_input_info.pVertexAttributeDescriptions = NULL;

  shader_init(&blit_effect->shader, vulkan_renderer, "./assets/shaders/spirv/screenspace.vert.spv", "./assets/shaders/spirv/blit.frag.spv", NULL, vertex_input_info, VK_FALSE);

  return 1;
}

void blit_effect_delete(struct BlitEffect* blit_effect, struct VulkanRenderer* vulkan_renderer) {
  shader_delete(&blit_effect->shader, vulkan_renderer);

  vkDestroyDescriptorPool(vulkan_renderer->device, blit_effect->shader.descriptor_pool, NULL);
}
