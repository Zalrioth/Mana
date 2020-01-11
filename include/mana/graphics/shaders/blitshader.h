#pragma once
#ifndef BLIT_EFFECT_H
#define BLIT_EFFECT_H

#include "mana/graphics/shaders/shader.h"

// Effect for blitting images

struct BlitShader {
  struct Shader shader;
};

int blit_shader_init(struct BlitShader* blit_effect, struct VulkanRenderer* vulkan_renderer, VkRenderPass render_pass, int descriptors);
void blit_shader_delete(struct BlitShader* blit_effect, struct VulkanRenderer* vulkan_renderer);

#endif  // BLIT_EFFECT_H