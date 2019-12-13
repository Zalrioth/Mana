#pragma once
#ifndef SHADER_H
#define SHADER_H

#include "core/common.h"
#include "core/vulkanrenderer.h"

struct Shader {
  struct VkPipelineLayout_T* pipeline_layout;
  struct VkPipeline_T* graphics_pipeline;
  struct VkDescriptorSetLayout_T* descriptor_set_layout;
  struct VkDescriptorSet_T* descriptor_sets[MAX_SWAP_CHAIN_FRAMES];
  struct VkBuffer_T* uniform_buffers[MAX_SWAP_CHAIN_FRAMES];
  struct VkDeviceMemory_T* uniform_buffers_memory[MAX_SWAP_CHAIN_FRAMES];
};

int shader_init(struct Shader* shader, struct VulkanRenderer* vulkan_renderer, char* vertex_shader, char* fragment_shader, char* geometry_shader);
void shader_delete(struct Shader* shader, struct VulkanRenderer* vulkan_renderer);
VkShaderModule shader_create_shader_module(struct VulkanRenderer* vulkan_renderer, const char* code, int length);

#endif