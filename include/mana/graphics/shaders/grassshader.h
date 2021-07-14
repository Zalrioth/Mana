#pragma once
#ifndef GRASS_SHADER_H
#define GRASS_SHADER_H

#include "mana/core/memoryallocator.h"
//
#include "mana/graphics/shaders/shader.h"

// Effect for drawing grass

#define GRASS_SHADER_COLOR_ATTACHEMENTS 2
#define GRASS_SHADER_VERTEX_ATTRIBUTES 5

struct GrassShader {
  struct Shader grass_compute_shader;
  struct Shader grass_render_shader;

  int buf_size[3];

  VkBuffer grass_compute_buffers[3];
  VkDeviceMemory grass_compute_memory;
  VkDescriptorSet descriptorSet;
  VkCommandPool commandPool;
  VkCommandBuffer commandBuffer;
  // Use ID system for grass to attach same grass nodes to specific blocks
  // Ping pong vbo on edit, either remake from scratch or update
  // Note: Will probably have to start with Wind Waker style grass to prevent crazy memory usage
};

int grass_shader_init(struct GrassShader* grass_shader, struct GPUAPI* gpu_api);
void grass_shader_delete(struct GrassShader* grass_effect, struct VulkanState* vulkan_renderer);

#endif  // GRASS_SHADER_H
