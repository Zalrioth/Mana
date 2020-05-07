#pragma once
#ifndef FXAA_SHADER_H
#define FXAA_SHADER_H

#include "mana/core/memoryallocator.h"
//
#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/render/vulkanrenderer.h"
#include "mana/graphics/shaders/shader.h"
#include "mana/graphics/utilities/fullscreenquad.h"

// Effect for full screen anti-aliasing

struct FXAAShader {
  struct Shader* shader;
  VkDescriptorSet descriptor_sets[2];
  struct FullscreenQuad* fullscreen_quad;
};

int fxaa_shader_init(struct FXAAShader* fxaa_shader, struct VulkanState* vulkan_renderer);
void fxaa_shader_delete(struct FXAAShader* fxaa_shader, struct VulkanState* vulkan_renderer);
void fxaa_shader_render(struct FXAAShader* fxaa_shader, struct VulkanState* vulkan_renderer);

#endif  // FXAA_SHADER_H
