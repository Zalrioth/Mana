#pragma once
#ifndef SPRITE_ANIMATION_SHADER_H
#define SPRITE_ANIMATION_SHADER_H

#include "mana/graphics/shaders/shader.h"

// Effect for blitting sprite animations to gbuffer

#define SPRITE_ANIMATION_SHADER_COLOR_ATTACHEMENTS 2
#define SPRITE_ANIMATION_SHADER_VERTEX_ATTRIBUTES 2

struct SpriteAnimationShader {
  struct Shader shader;
};

int sprite_animation_shader_init(struct SpriteAnimationShader* sprite_animation_shader, struct GPUAPI* gpu_api);
void sprite_animation_shader_delete(struct SpriteAnimationShader* sprite_animation_shader, struct GPUAPI* gpu_api);

#endif  // SPRITE_ANIMATION_SHADER_H
