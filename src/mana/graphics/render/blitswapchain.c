#include "mana/graphics/render/blitswapchain.h"

int blit_swapchain_init(struct BlitSwapchain* blit_swapchain, struct VulkanRenderer* vulkan_renderer, struct Shader* shader) {
  sprite_init(&blit_swapchain->sprite, vulkan_renderer);

  VkDescriptorSetLayout layout = {0};
  layout = shader->descriptor_set_layout;

  VkDescriptorSetAllocateInfo alloc_info = {0};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = shader->descriptor_pool;
  alloc_info.descriptorSetCount = 1;
  alloc_info.pSetLayouts = &layout;

  if (vkAllocateDescriptorSets(vulkan_renderer->device, &alloc_info, &blit_swapchain->sprite.descriptor_set) != VK_SUCCESS) {
    fprintf(stderr, "failed to allocate descriptor sets!\n");
    return 0;
  }

  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
    VkDescriptorImageInfo image_info = {0};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = blit_swapchain->sprite.image_texture->texture_image_view;
    image_info.sampler = blit_swapchain->sprite.image_texture->texture_sampler;

    VkWriteDescriptorSet dc;
    memset(&dc, 0, sizeof(dc));

    dc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    dc.dstSet = blit_swapchain->sprite.descriptor_set;
    dc.dstBinding = 0;
    dc.dstArrayElement = 0;
    dc.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    dc.descriptorCount = 1;
    dc.pImageInfo = &image_info;

    vkUpdateDescriptorSets(vulkan_renderer->device, 1, &dc, 0, NULL);
  }

  return 1;
}

void blit_swapchain_delete(struct BlitSwapchain* blit_swapchain, struct VulkanRenderer* vulkan_renderer) {
  sprite_delete(&blit_swapchain->sprite, vulkan_renderer);
}