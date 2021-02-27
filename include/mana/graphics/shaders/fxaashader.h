#pragma once
#ifndef FXAA_SHADER_H
#define FXAA_SHADER_H

#include "mana/core/memoryallocator.h"
//
#include <mana/core/gpuapi.h>

#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/render/vulkanrenderer.h"
#include "mana/graphics/shaders/shader.h"
#include "mana/graphics/utilities/fullscreentriangle.h"

// Effect for full screen anti-aliasing

struct FXAAShader {
  struct Shader* shader;
  VkDescriptorSet descriptor_sets[2];
  struct FullscreenTriangle* fullscreen_triangle;
  int fxaa_on;
};

int fxaa_shader_init(struct FXAAShader* fxaa_shader, struct GPUAPI* gpu_api);
void fxaa_shader_delete(struct FXAAShader* fxaa_shader, struct GPUAPI* gpu_api);
void fxaa_shader_render(struct FXAAShader* fxaa_shader, struct GPUAPI* gpu_api);

#endif  // FXAA_SHADER_H
