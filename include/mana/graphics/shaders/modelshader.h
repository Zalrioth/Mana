#pragma once
#ifndef MODEL_SHADER_H
#define MODEL_SHADER_H

#include "mana/graphics/shaders/shader.h"

// Effect for blitting model to gbuffer

#define MODEL_SHADER_COLOR_ATTACHEMENTS 2
#define MODEL_SHADER_VERTEX_ATTRIBUTES 6

struct ModelShader {
  struct Shader shader;
};

int model_shader_init(struct ModelShader* model_shader, struct GPUAPI* gpu_api);
void model_shader_delete(struct ModelShader* model_shader, struct GPUAPI* gpu_api);

#endif  // MODEL_SHADER_H