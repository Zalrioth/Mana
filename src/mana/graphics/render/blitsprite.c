#include "mana/graphics/render/blitsprite.h"

int blit_sprite_init(struct BlitSprite* blit_sprite, struct VulkanRenderer* vulkan_renderer, struct Shader* shader) {
  sprite_init(&blit_sprite->sprite, vulkan_renderer);

  VkDescriptorSetLayout layouts[MAX_SWAP_CHAIN_FRAMES];
  memset(layouts, 0, sizeof(layouts));

  for (int loopNum = 0; loopNum < MAX_SWAP_CHAIN_FRAMES; loopNum++)
    layouts[loopNum] = shader->descriptor_set_layout;

  VkDescriptorSetAllocateInfo alloc_info = {0};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = shader->descriptor_pool;
  alloc_info.descriptorSetCount = MAX_SWAP_CHAIN_FRAMES;
  alloc_info.pSetLayouts = layouts;

  if (vkAllocateDescriptorSets(vulkan_renderer->device, &alloc_info, &blit_sprite->sprite.descriptor_set) != VK_SUCCESS) {
    fprintf(stderr, "failed to allocate descriptor sets!\n");
    return 0;
  }

  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
    VkDescriptorImageInfo image_info = {0};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = blit_sprite->sprite.image_texture->texture_image_view;
    image_info.sampler = blit_sprite->sprite.image_texture->texture_sampler;

    VkWriteDescriptorSet dc;
    memset(&dc, 0, sizeof(dc));

    dc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    dc.dstSet = blit_sprite->sprite.descriptor_set;
    dc.dstBinding = 0;
    dc.dstArrayElement = 0;
    dc.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    dc.descriptorCount = 1;
    dc.pImageInfo = &image_info;

    vkUpdateDescriptorSets(vulkan_renderer->device, 1, &dc, 0, NULL);
  }

  return 1;
}

/*int blit_sprite_init(struct BlitSprite* blit_sprite, struct VulkanRenderer* vulkan_renderer, struct Shader* shader) {
  sprite_init(&blit_sprite->sprite, vulkan_renderer);

  VkDescriptorSetLayout layouts[MAX_SWAP_CHAIN_FRAMES];
  memset(layouts, 0, sizeof(layouts));

  for (int loopNum = 0; loopNum < MAX_SWAP_CHAIN_FRAMES; loopNum++)
    layouts[loopNum] = shader->descriptor_set_layout;

  VkDescriptorSetAllocateInfo alloc_info = {0};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = shader->descriptor_pool;
  alloc_info.descriptorSetCount = MAX_SWAP_CHAIN_FRAMES;
  alloc_info.pSetLayouts = layouts;

  if (vkAllocateDescriptorSets(vulkan_renderer->device, &alloc_info, blit_sprite->sprite.descriptor_sets) != VK_SUCCESS) {
    fprintf(stderr, "failed to allocate descriptor sets!\n");
    return 0;
  }

  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
    VkDescriptorBufferInfo buffer_info = {0};
    buffer_info.buffer = blit_sprite->sprite.uniform_buffers[i];
    buffer_info.offset = 0;
    buffer_info.range = sizeof(struct UniformBufferObject);

    VkDescriptorImageInfo image_info = {0};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = blit_sprite->sprite.image_texture->texture_image_view;
    image_info.sampler = blit_sprite->sprite.image_texture->texture_sampler;

    VkWriteDescriptorSet dcs[2];
    memset(dcs, 0, sizeof(dcs));

    dcs[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    dcs[0].dstSet = blit_sprite->sprite.descriptor_sets[i];
    dcs[0].dstBinding = 0;
    dcs[0].dstArrayElement = 0;
    dcs[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    dcs[0].descriptorCount = 1;
    dcs[0].pBufferInfo = &buffer_info;

    dcs[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    dcs[1].dstSet = blit_sprite->sprite.descriptor_sets[i];
    dcs[1].dstBinding = 1;
    dcs[1].dstArrayElement = 0;
    dcs[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    dcs[1].descriptorCount = 1;
    dcs[1].pImageInfo = &image_info;

    vkUpdateDescriptorSets(vulkan_renderer->device, 2, dcs, 0, NULL);
  }

  return 1;
}*/

void blit_sprite_delete(struct BlitSprite* blit_sprite, struct VulkanRenderer* vulkan_renderer) {
  sprite_delete(&blit_sprite->sprite, vulkan_renderer);
}