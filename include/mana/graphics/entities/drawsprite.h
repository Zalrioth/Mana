#pragma once
#ifndef DRAW_SPRITE_H
#define DRAW_SPRITE_H

#include "mana/graphics/entities/sprite.h"
#include "mana/graphics/shaders/shader.h"

struct DrawSprite {
  struct Sprite sprite;
};

int draw_sprite_init(struct DrawSprite* blit_sprite, struct VulkanRenderer* vulkan_renderer, struct Shader* shader);
void draw_sprite_delete(struct DrawSprite* blit_sprite, struct VulkanRenderer* vulkan_renderer);

#endif  // DRAW_SPRITE_H