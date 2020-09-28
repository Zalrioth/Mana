#include "mana/graphics/utilities/fullscreentriangle.h"

void fullscreen_triangle_init(struct FullscreenTriangle* fullscreen_triangle, struct GPUAPI* gpu_api) {
  fullscreen_triangle->mesh = calloc(1, sizeof(struct Mesh));
  mesh_triangle_init(fullscreen_triangle->mesh);

  graphics_utils_setup_vertex_buffer(gpu_api->vulkan_state, fullscreen_triangle->mesh->vertices, &fullscreen_triangle->vertex_buffer, &fullscreen_triangle->vertex_buffer_memory);
  graphics_utils_setup_index_buffer(gpu_api->vulkan_state, fullscreen_triangle->mesh->indices, &fullscreen_triangle->index_buffer, &fullscreen_triangle->index_buffer_memory);
}

void fullscreen_triangle_delete(struct FullscreenTriangle* fullscreen_triangle, struct GPUAPI* gpu_api) {
  vkDestroyBuffer(gpu_api->vulkan_state->device, fullscreen_triangle->index_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, fullscreen_triangle->index_buffer_memory, NULL);

  vkDestroyBuffer(gpu_api->vulkan_state->device, fullscreen_triangle->vertex_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, fullscreen_triangle->vertex_buffer_memory, NULL);

  mesh_delete(fullscreen_triangle->mesh);
  free(fullscreen_triangle->mesh);
}
