#pragma once
#ifndef FULLSCREEN_TRIANGLE_H
#define FULLSCREEN_TRIANGLE_H

#include "mana/core/memoryallocator.h"
//
#include "mana/graphics/utilities/mesh.h"

struct FullscreenTriangle {
  struct Mesh* mesh;
  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_buffer_memory;
  VkBuffer index_buffer;
  VkDeviceMemory index_buffer_memory;
};

void fullscreen_triangle_init(struct FullscreenTriangle* fullscreen_triangle, struct GPUAPI* gpu_api);
void fullscreen_triangle_delete(struct FullscreenTriangle* fullscreen_triangle, struct GPUAPI* gpu_api);

#endif  // FULLSCREEN_TRIANGLE_H