#pragma once
#ifndef SPRITE_SHADER_H
#define SPRITE_SHADER_H

#include "mana/graphics/shaders/shader.h"

// Effect for blitting sprite to gbuffer

#define SPRITE_SHADER_COLOR_ATTACHEMENTS 2
#define SPRITE_SHADER_VERTEX_ATTRIBUTES 2

struct SpriteShader {
  struct Shader shader;
};

int sprite_shader_init(struct SpriteShader* sprite_shader, struct GPUAPI* gpu_api);
void sprite_shader_delete(struct SpriteShader* sprite_shader, struct GPUAPI* gpu_api);

#endif  // SPRITE_SHADER_H
