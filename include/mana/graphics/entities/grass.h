#pragma once
#ifndef GRASS_H
#define GRASS_H

#include "mana/core/memoryallocator.h"
//
#include "mana/graphics/shaders/grassshader.h"

struct GrassUniformBufferObject {
  alignas(16) mat4 model;
  alignas(16) mat4 view;
  alignas(16) mat4 proj;
};

struct Grass {
  struct GrassShader grass_shader;

  struct Mesh* mesh;
  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_buffer_memory;
  VkBuffer index_buffer;
  VkDeviceMemory index_buffer_memory;
  VkBuffer uniform_buffer;
  VkDeviceMemory uniform_buffers_memory;
  VkDescriptorSet descriptor_set;

  struct Vector grass_nodes;
};

int grass_init(struct Grass* grass, struct GPUAPI* gpu_api);
void grass_delete(struct Grass* grass, struct VulkanState* vulkan_renderer);
void grass_render(struct Grass* grass, struct GPUAPI* gpu_api);
void grass_update_uniforms(struct Grass* grass, struct GPUAPI* gpu_api);

#endif  // GRASS_H
