#pragma once
#ifndef GRAPHICS_COMMON_H
#define GRAPHICS_COMMON_H

#define MAX_FRAMES_IN_FLIGHT 2
#define MAX_SWAP_CHAIN_FRAMES MAX_FRAMES_IN_FLIGHT + 1

#define Z_FAR 10000000.0f
#define Z_NEAR 0.01f

#include <cglm/cglm.h>
#include <stdalign.h>

struct LightingUniformBufferObject {
  alignas(16) vec3 direction;
  alignas(16) vec3 ambient_color;
  alignas(16) vec3 diffuse_colour;
  alignas(16) vec3 specular_colour;
};

#endif  // GRAPHICS_COMMON_H
