#pragma once
#ifndef MODEL_SHADER_H
#define MODEL_SHADER_H

#include "mana/graphics/shaders/shader.h"

// Effect for blitting model to gbuffer

#define MODEL_SHADER_COLOR_ATTACHEMENTS 2
#define MODEL_SHADER_VERTEX_ATTRIBUTES 5

struct ModelShader {
  struct Shader shader;
};

int model_shader_init(struct ModelShader* model_shader, struct VulkanRenderer* vulkan_renderer);
void model_shader_delete(struct ModelShader* model_shader, struct VulkanRenderer* vulkan_renderer);

#endif  // MODEL_SHADER_H