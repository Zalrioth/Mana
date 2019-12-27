#pragma once
#ifndef BLIT_EFFECT_H
#define BLIT_EFFECT_H

#include "mana/graphics/shader.h"

// Effect for blitting images to swapchain

struct BlitEffect {
  struct Shader shader;
};

int blit_effect_init(struct BlitEffect* blit_effect, struct VulkanRenderer* vulkan_renderer);
void blit_effect_delete(struct BlitEffect* blit_effect, struct VulkanRenderer* vulkan_renderer);

#endif  // BLIT_EFFECT_H