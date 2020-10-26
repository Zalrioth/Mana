#pragma once
#ifndef BLOOM_EFFECT_H
#define BLOOM_EFFECT_H

#include "mana/core/memoryallocator.h"
//
#include "mana/graphics/shaders/shader.h"

// Effect for blitting images

struct BloomShader {
  struct Shader* bloom_shader;
  struct Shader* bloom_threshold_shader;
  struct Shader* bloom_blur_shader;
};

int bloom_shader_init(struct BloomShader* bloom_shader, struct VulkanState* vulkan_renderer, VkRenderPass render_pass, int descriptors);
void bloom_shader_delete(struct BloomShader* bloom_shader, struct VulkanState* vulkan_renderer);

#endif  // BLOOM_EFFECT_H
