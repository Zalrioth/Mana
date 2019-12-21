#pragma once
#ifndef BLIT_SPRITE_H
#define BLIT_SPRITE_H

#include "graphics/render/sprite.h"
#include "graphics/shader.h"

struct BlitSprite {
  struct Sprite sprite;
};

int blit_sprite_init(struct BlitSprite* blit_sprite, struct VulkanRenderer* vulkan_renderer, struct Shader* shader);
void blit_sprite_delete(struct BlitSprite* blit_sprite, struct VulkanRenderer* vulkan_renderer);

#endif  // BLIT_SPRITE_H