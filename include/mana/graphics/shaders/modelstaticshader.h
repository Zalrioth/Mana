#pragma once
#ifndef MODEL_STATIC_SHADER_H
#define MODEL_STATIC_SHADER_H

#include "mana/graphics/shaders/shader.h"

// Effect for blitting static model to gbuffer

#define MODEL_STATIC_SHADER_COLOR_ATTACHEMENTS 2
#define MODEL_STATIC_SHADER_VERTEX_ATTRIBUTES 4

struct ModelStaticShader {
  struct Shader shader;
};

int model_static_shader_init(struct ModelStaticShader* model_static_shader, struct GPUAPI* gpu_api);
void model_static_shader_delete(struct ModelStaticShader* model_static_shader, struct GPUAPI* gpu_api);

#endif  // MODEL_STATIC_SHADER_H