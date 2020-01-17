#pragma once
#ifndef SPRITE_SHADER_H
#define SPRITE_SHADER_H

#include "mana/graphics/shaders/shader.h"

// Effect for blitting sprite to gbuffer

#define SPRITE_SHADER_COLOR_ATTACHEMENTS 2

struct SpriteShader {
  struct Shader shader;
};

int sprite_shader_init(struct SpriteShader* sprite_shader, struct VulkanRenderer* vulkan_renderer);
void sprite_shader_delete(struct SpriteShader* sprite_shader, struct VulkanRenderer* vulkan_renderer);

#endif  // SPRITE_SHADER_H