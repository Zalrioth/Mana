#pragma once
#ifndef MANIFOLD_DUAL_CONTOURING_SHADER_H
#define MANIFOLD_DUAL_CONTOURING_SHADER_H

#include "mana/graphics/shaders/shader.h"

// Effect for blitting dual contouring

#define MANIFOLD_DUAL_CONTOURING_COLOR_ATTACHEMENTS 2
#define MANIFOLD_DUAL_CONTOURING_VERTEX_ATTRIBUTES 4

struct ManifoldDualContouringShader {
  struct Shader shader;
};

int manifold_dual_contouring_shader_init(struct ManifoldDualContouringShader* manifold_dual_countouring_shader, struct GPUAPI* gpu_api);
void manifold_dual_contouring_shader_delete(struct ManifoldDualContouringShader* manifold_dual_countouring_shader, struct GPUAPI* gpu_api);

#endif  // MANIFOLD_DUAL_CONTOURING_SHADER_H
