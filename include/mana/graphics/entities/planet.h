#pragma once
#ifndef PLANET_H
#define PLANET_H

#include "mana/core/memoryallocator.h"
//
#include <mana/core/gpuapi.h>

#include "mana/graphics/dualcontouring/dualcontouring.h"
#include "mana/graphics/utilities/camera.h"

// TODO: Should take care of LOD

enum PlanetType {
  ROUND_PLANET
};

struct Planet {
  enum PlanetType planet_type;
  struct DualContouring dual_contouring;
  struct Shader* terrain_shader;
};

void planet_init(struct Planet* planet, struct GPUAPI* gpu_api, size_t octree_size, struct Shader* shader, struct Vector* noises, float (*density_func_single)(struct Vector*, float, float, float), float* (*density_func_set)(struct Vector*, float, float, float, int, int, int));
void planet_delete(struct Planet* planet, struct GPUAPI* gpu_api);
void planet_render(struct Planet* planet, struct GPUAPI* gpu_api);
void planet_update_uniforms(struct Planet* planet, struct GPUAPI* gpu_api, struct Camera* camera, vec3 light_pos);

#endif  // PLANET_H
