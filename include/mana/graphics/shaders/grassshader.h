#pragma once
#ifndef GRASS_EFFECT_H
#define GRASS_EFFECT_H

#include "mana/core/memoryallocator.h"
//
#include "mana/graphics/shaders/shader.h"

// Effect for drawing grass

struct GrassShader {
  struct Shader* grass_compute_shader;
  struct Shader* grass_render_shader;

  // Use ID system for grass to attach same grass nodes to specific blocks
  // Ping pong vbo on edit, either remake from scratch or update
  // Note: Will probably have to start with Wind Waker style grass to prevent crazy memory usage
  struct Map* grass_nodes;
};

int grass_shader_init(struct GrassShader* grass_shader, struct GPUAPI* gpu_api);
void grass_shader_delete(struct GrassShader* grass_effect, struct VulkanState* vulkan_renderer);

#endif  // GRASS_EFFECT_H
