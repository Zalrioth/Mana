#include "mana/graphics/utilities/fullscreenquad.h"

void fullscreen_quad_init(struct FullscreenQuad* fullscreen_quad, struct VulkanState* vulkan_renderer) {
  fullscreen_quad->mesh = calloc(1, sizeof(struct Mesh));
  mesh_quad_init(fullscreen_quad->mesh);

  graphics_utils_setup_vertex_buffer(vulkan_renderer, fullscreen_quad->mesh->vertices, &fullscreen_quad->vertex_buffer, &fullscreen_quad->vertex_buffer_memory);
  graphics_utils_setup_index_buffer(vulkan_renderer, fullscreen_quad->mesh->indices, &fullscreen_quad->index_buffer, &fullscreen_quad->index_buffer_memory);
}

void fullscreen_quad_delete(struct FullscreenQuad* fullscreen_quad, struct VulkanState* vulkan_renderer) {
  vkDestroyBuffer(vulkan_renderer->device, fullscreen_quad->index_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, fullscreen_quad->index_buffer_memory, NULL);

  vkDestroyBuffer(vulkan_renderer->device, fullscreen_quad->vertex_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, fullscreen_quad->vertex_buffer_memory, NULL);

  mesh_delete(fullscreen_quad->mesh);
  free(fullscreen_quad->mesh);
}
