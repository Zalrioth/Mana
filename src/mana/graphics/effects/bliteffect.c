#include "mana/graphics/effects/bliteffect.h"

int blit_effect_init(struct BlitEffect* blit_effect, struct VulkanRenderer* vulkan_renderer) {
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

  if (vkCreateDescriptorSetLayout(vulkan_renderer->device, &layout_info, NULL, &blit_effect->shader.descriptor_set_layout) != VK_SUCCESS)
    return 0;

  VkDescriptorPoolSize pool_sizes[2];
  memset(pool_sizes, 0, sizeof(pool_sizes));

  int sprite_descriptors = 100;

  pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  pool_sizes[0].descriptorCount = sprite_descriptors;  // Max number of uniform descriptors
  pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  pool_sizes[1].descriptorCount = sprite_descriptors;  // Max number of image sampler descriptors

  // "We preallocate pretty much the maximum number of sets on the CPU + space for descriptors on the GPU."
  // https://www.reddit.com/r/vulkan/comments/839d15/vkdescriptorpool_best_practices/
  // https://vulkan.lunarg.com/doc/view/1.0.26.0/linux/vkspec.chunked/ch13s02.html
  VkDescriptorPoolCreateInfo poolInfo = {0};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 2;  // Number of things being passed to GPU
  poolInfo.pPoolSizes = pool_sizes;
  poolInfo.maxSets = sprite_descriptors;  // Max number of sets made from this pool

  if (vkCreateDescriptorPool(vulkan_renderer->device, &poolInfo, NULL, &blit_effect->shader.descriptor_pool) != VK_SUCCESS) {
    fprintf(stderr, "failed to create descriptor pool!\n");
    return 0;
  }

  shader_init(&blit_effect->shader, vulkan_renderer, "./assets/shaders/spirv/texture.vert.spv", "./assets/shaders/spirv/texture.frag.spv", NULL);

  return 1;
}

void blit_effect_delete(struct BlitEffect* blit_effect, struct VulkanRenderer* vulkan_renderer) {
  shader_delete(&blit_effect->shader, vulkan_renderer);

  vkDestroyDescriptorPool(vulkan_renderer->device, blit_effect->shader.descriptor_pool, NULL);
}
