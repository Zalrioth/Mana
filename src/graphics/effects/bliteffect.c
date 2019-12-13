#include "graphics/effects/bliteffect.h"

void blit_effect_init(struct BlitEffect* blit_effect, struct VulkanRenderer* vulkan_renderer) {
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

  shader_init(&blit_effect->shader, vulkan_renderer, "./assets/shaders/spirv/texture.vert.spv", "./assets/shaders/spirv/texture.frag.spv", NULL);

  /////////////////////////////////////////////////////////////////////////
  VkDescriptorSetLayout layouts[MAX_SWAP_CHAIN_FRAMES];
  memset(layouts, 0, sizeof(layouts));

  for (int loopNum = 0; loopNum < MAX_SWAP_CHAIN_FRAMES; loopNum++)
    layouts[loopNum] = blit_effect->shader.descriptor_set_layout;

  VkDescriptorSetAllocateInfo alloc_info = {0};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = vulkan_renderer->descriptor_pool;
  alloc_info.descriptorSetCount = MAX_SWAP_CHAIN_FRAMES;
  alloc_info.pSetLayouts = layouts;

  if (vkAllocateDescriptorSets(vulkan_renderer->device, &alloc_info, blit_effect->shader.descriptor_sets) != VK_SUCCESS) {
    fprintf(stderr, "failed to allocate descriptor sets!\n");
    return -1;
  }

  /////////////////////////////////////////////////////////////////////////
  VkSamplerCreateInfo samplerInfo = {0};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = 16;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  if (vkCreateSampler(vulkan_renderer->device, &samplerInfo, NULL, &blit_effect->shader.texture_sampler) != VK_SUCCESS)
    return 0;

  //blit_effect->shader.texture_image_view = graphics_utils_create_image_view(vulkan_renderer->device, texture->textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
    VkDescriptorBufferInfo buffer_info = {0};
    buffer_info.buffer = blit_effect->shader.uniform_buffers[i];
    buffer_info.offset = 0;
    buffer_info.range = sizeof(struct UniformBufferObject);

    VkDescriptorImageInfo image_info = {0};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = blit_effect->shader.texture_image_view;
    image_info.sampler = blit_effect->shader.texture_sampler;

    VkWriteDescriptorSet dcs[2];
    memset(dcs, 0, sizeof(dcs));

    dcs[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    dcs[0].dstSet = sprite->descriptor_sets[i];
    dcs[0].dstBinding = 0;
    dcs[0].dstArrayElement = 0;
    dcs[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    dcs[0].descriptorCount = 1;
    dcs[0].pBufferInfo = &buffer_info;

    dcs[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    dcs[1].dstSet = sprite->descriptor_sets[i];
    dcs[1].dstBinding = 1;
    dcs[1].dstArrayElement = 0;
    dcs[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    dcs[1].descriptorCount = 1;
    dcs[1].pImageInfo = &image_info;

    vkUpdateDescriptorSets(vulkan_renderer->device, 2, dcs, 0, NULL);
  }
  return 1;
}

void blit_effect_delete(struct BlitEffect* blit_effect, struct VulkanRenderer* vulkan_renderer) {
  shader_delete(&blit_effect->shader, vulkan_renderer);
}
