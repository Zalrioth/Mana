#pragma once
#ifndef SHADER_H
#define SHADER_H

#include "mana/core/common.h"
#include "mana/core/vulkanrenderer.h"

struct Shader {
  struct VkPipelineLayout_T* pipeline_layout;
  struct VkPipeline_T* graphics_pipeline;
  struct VkDescriptorPool_T* descriptor_pool;
  struct VkDescriptorSetLayout_T* descriptor_set_layout;
};

int shader_init(struct Shader* shader, struct VulkanRenderer* vulkan_renderer, char* vertex_shader, char* fragment_shader, char* geometry_shader);
void shader_delete(struct Shader* shader, struct VulkanRenderer* vulkan_renderer);
VkShaderModule shader_create_shader_module(struct VulkanRenderer* vulkan_renderer, const char* code, int length);
int sprite_create_sprite_descriptor_pool(struct Shader* shader, struct VulkanRenderer* vulkan_renderer);

#endif