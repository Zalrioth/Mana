#pragma once
#ifndef ATMOSPHERIC_SCATTERING_H
#define ATMOSPHERIC_SCATTERING_H

#include "mana/core/memoryallocator.h"
//
#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/render/vulkanrenderer.h"
#include "mana/graphics/shaders/shader.h"
#include "mana/graphics/utilities/fullscreenquad.h"

// Effect for atmospheric scattering

struct AtmosphericScatteringUniformBufferObject {
  alignas(32) mat4 model_from_view;
  vec3 camera;
};

struct AtmosphericScatteringUniformBufferObjectSettings {
  alignas(32) mat4 view_from_clip;  // Based on window size, will need to update on resize
  float exposure;
  vec3 white_point;
  vec3 earth_center;
  vec3 sun_direction;
  vec2 sun_size;
};

struct AtmosphericScatteringShader {
  struct Shader* shader;
  VkDescriptorSet descriptor_set;
  struct FullscreenQuad* fullscreen_quad;

  VkBuffer uniform_buffer;
  VkDeviceMemory uniform_buffer_memory;
  VkBuffer uniform_buffer_settings;
  VkDeviceMemory uniform_buffer_settings_memory;
};

int atmospheric_scattering_shader_init(struct AtmosphericScatteringShader* atmospheric_scattering_shader, struct VulkanState* vulkan_renderer);
void atmospheric_scattering_shader_delete(struct AtmosphericScatteringShader* atmospheric_scattering_shader, struct VulkanState* vulkan_renderer);
void atmospheric_scattering_shader_render(struct AtmosphericScatteringShader* atmospheric_scattering_shader, struct VulkanState* vulkan_renderer);

#endif  // ATMOSPHERIC_SCATTERING_H
