#pragma once
#ifndef SPRITE_EFFECT_H
#define SPRITE_EFFECT_H

#include "mana/graphics/shader.h"

// Effect for blitting sprite to gbuffer

struct SpriteEffect {
  struct Shader shader;
};

int sprite_effect_init(struct SpriteEffect* blit_effect, struct VulkanRenderer* vulkan_renderer);
void sprite_effect_delete(struct SpriteEffect* blit_effect, struct VulkanRenderer* vulkan_renderer);

#endif  // SPRITE_EFFECT_H