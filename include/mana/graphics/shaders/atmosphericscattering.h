#pragma once
#ifndef ATMOSPHERIC_SCATTERING_H
#define ATMOSPHERIC_SCATTERING_H

#include "mana/core/memoryallocator.h"
//
#include <math.h>

#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/render/vulkanrenderer.h"
#include "mana/graphics/shaders/shader.h"
#include "mana/graphics/utilities/fullscreentriangle.h"

// Effect for atmospheric scattering

#define MANBDA_MIN = 360;
#define MANBDA_MAX = 830;

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
  struct FullscreenTriangle* fullscreen_triangle;

  VkBuffer uniform_buffer;
  VkDeviceMemory uniform_buffer_memory;
  VkBuffer uniform_buffer_settings;
  VkDeviceMemory uniform_buffer_settings_memory;

  float sun_angle;
};

int atmospheric_scattering_shader_init(struct AtmosphericScatteringShader* atmospheric_scattering_shader, struct GPUAPI* gpu_api);
void atmospheric_scattering_shader_delete(struct AtmosphericScatteringShader* atmospheric_scattering_shader, struct GPUAPI* gpu_api);
void atmospheric_scattering_shader_render(struct AtmosphericScatteringShader* atmospheric_scattering_shader, struct GPUAPI* gpu_api);

#endif  // ATMOSPHERIC_SCATTERING_H
