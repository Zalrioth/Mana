#pragma once
#ifndef GRAPHICS_COMMON_H
#define GRAPHICS_COMMON_H

#define MAX_FRAMES_IN_FLIGHT 2
#define MAX_SWAP_CHAIN_FRAMES MAX_FRAMES_IN_FLIGHT + 1

#define Z_FAR 10000000.0f
#define Z_NEAR 0.01f

#include <stdalign.h>
#include <ubermath/ubermath.h>

struct LightingUniformBufferObject {
  vec3 direction;
  vec3 ambient_color;
  vec3 diffuse_colour;
  vec3 specular_colour;
};

#endif  // GRAPHICS_COMMON_H
