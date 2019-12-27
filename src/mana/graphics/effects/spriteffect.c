#include "mana/graphics/effects/spriteeffect.h"

int sprite_effect_init(struct SpriteEffect* sprite_effect, struct VulkanRenderer* vulkan_renderer) {
  VkDescriptorSetLayoutBinding ubo_layout_binding = {0};
  ubo_layout_binding.binding = 0;
  ubo_layout_binding.descriptorCount = 1;
  ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  ubo_layout_binding.pImmutableSamplers = NULL;
  ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding sampler_layout_binding = {0};
  sampler_layout_binding.binding = 1;
  sampler_layout_binding.descriptorCount = 1;
  sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  sampler_layout_binding.pImmutableSamplers = NULL;
  sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding bindings[2] = {ubo_layout_binding, sampler_layout_binding};
  VkDescriptorSetLayoutCreateInfo layout_info = {0};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = 2;
  layout_info.pBindings = bindings;

  if (vkCreateDescriptorSetLayout(vulkan_renderer->device, &layout_info, NULL, &sprite_effect->shader.descriptor_set_layout) != VK_SUCCESS)
    return 0;

  VkDescriptorPoolSize pool_sizes[2];
  memset(pool_sizes, 0, sizeof(pool_sizes));

  int sprite_descriptors = 100;

  pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  pool_sizes[0].descriptorCount = sprite_descriptors;  // Max number of uniform descriptors
  pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  pool_sizes[1].descriptorCount = sprite_descriptors;  // Max number of image sampler descriptors

  VkDescriptorPoolCreateInfo poolInfo = {0};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 2;  // Number of things being passed to GPU
  poolInfo.pPoolSizes = pool_sizes;
  poolInfo.maxSets = sprite_descriptors;  // Max number of sets made from this pool

  if (vkCreateDescriptorPool(vulkan_renderer->device, &poolInfo, NULL, &sprite_effect->shader.descriptor_pool) != VK_SUCCESS) {
    fprintf(stderr, "failed to create descriptor pool!\n");
    return 0;
  }

  VkPipelineVertexInputStateCreateInfo vertex_input_info = {0};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  VkVertexInputBindingDescription binding_description = get_binding_description();
  VkVertexInputAttributeDescription attribute_descriptions[5];
  memset(attribute_descriptions, 0, sizeof(attribute_descriptions));
  get_attribute_descriptions(attribute_descriptions);

  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.vertexAttributeDescriptionCount = 5;  // Note: length of attributeDescriptions
  vertex_input_info.pVertexBindingDescriptions = &binding_description;
  vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions;

  shader_init(&sprite_effect->shader, vulkan_renderer, "./assets/shaders/spirv/texture.vert.spv", "./assets/shaders/spirv/texture.frag.spv", NULL, vertex_input_info, VK_TRUE);

  return 1;
}

void sprite_effect_delete(struct SpriteEffect* sprite_effect, struct VulkanRenderer* vulkan_renderer) {
  shader_delete(&sprite_effect->shader, vulkan_renderer);

  vkDestroyDescriptorPool(vulkan_renderer->device, sprite_effect->shader.descriptor_pool, NULL);
}
