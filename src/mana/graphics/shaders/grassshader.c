#include "mana/graphics/shaders/grassshader.h"

int grass_shader_init(struct GrassShader* grass_shader, struct GPUAPI* gpu_api) {
  grass_shader->grass_compute_shader = calloc(1, sizeof(struct Shader));

  VkDescriptorSetLayoutBinding layout_bindings[3] = {0};

  for (uint32_t i = 0; i < 3; i++) {
    VkDescriptorSetLayoutBinding layout_binding = {0};
    layout_binding.binding = i;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    layout_bindings[i] = layout_binding;
  }

  VkDescriptorSetLayoutCreateInfo set_layout_create_info = {0};
  set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  set_layout_create_info.bindingCount = 3;
  set_layout_create_info.pBindings = layout_bindings;

  if (vkCreateDescriptorSetLayout(gpu_api->vulkan_state->device, &set_layout_create_info, NULL, &grass_shader->grass_compute_shader->descriptor_set_layout) != VK_SUCCESS)
    return -1;

  shader_init_comp(grass_shader->grass_compute_shader, gpu_api->vulkan_state, "./assets/shaders/spirv/grassvert.comp.spv");

  return 1;
}

void grass_shader_delete(struct GrassShader* grass_shader, struct VulkanState* vulkan_renderer) {
  vkDestroyDescriptorPool(vulkan_renderer->device, grass_shader->grass_compute_shader->descriptor_pool, NULL);
  shader_delete(grass_shader->grass_compute_shader, vulkan_renderer);
  free(grass_shader->grass_compute_shader);
}
