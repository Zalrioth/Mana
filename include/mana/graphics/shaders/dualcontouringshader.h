#pragma once
#ifndef DUAL_CONTOURING_SHADER_H
#define DUAL_CONTOURING_SHADER_H

#include "mana/graphics/shaders/shader.h"

// Effect for blitting dual contouring

#define DUAL_CONTOURING_COLOR_ATTACHEMENTS 2
#define DUAL_CONTOURING_VERTEX_ATTRIBUTES 2

struct DualContouringShader {
  struct Shader shader;
};

int dual_contouring_shader_init(struct DualContouringShader* dual_countouring_shader, struct GPUAPI* gpu_api);
void dual_contouring_shader_delete(struct DualContouringShader* dual_countouring_shader, struct GPUAPI* gpu_api);

#endif  // DUAL_CONTOURING_SHADER_H