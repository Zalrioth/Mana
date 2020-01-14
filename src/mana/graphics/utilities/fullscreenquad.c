#include "mana/graphics/utilities/fullscreenquad.h"

void fullscreen_quad_init(struct FullscreenQuad* fullscreen_quad, struct VulkanRenderer* vulkan_renderer) {
  fullscreen_quad->mesh = calloc(1, sizeof(struct Mesh));
  mesh_init(fullscreen_quad->mesh);

  mesh_assign_vertex(fullscreen_quad->mesh->vertices, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  mesh_assign_vertex(fullscreen_quad->mesh->vertices, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
  mesh_assign_vertex(fullscreen_quad->mesh->vertices, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
  mesh_assign_vertex(fullscreen_quad->mesh->vertices, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

  mesh_assign_indice(fullscreen_quad->mesh->indices, 0);
  mesh_assign_indice(fullscreen_quad->mesh->indices, 1);
  mesh_assign_indice(fullscreen_quad->mesh->indices, 2);
  mesh_assign_indice(fullscreen_quad->mesh->indices, 2);
  mesh_assign_indice(fullscreen_quad->mesh->indices, 3);
  mesh_assign_indice(fullscreen_quad->mesh->indices, 0);

  // Vertex
  VkDeviceSize buffer_size_vertex = fullscreen_quad->mesh->vertices->memory_size * fullscreen_quad->mesh->vertices->size;
  VkBuffer staging_buffer_vertex = {0};
  VkDeviceMemory staging_buffer_memory_index = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size_vertex, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer_vertex, &staging_buffer_memory_index);

  void* data_vertex;
  vkMapMemory(vulkan_renderer->device, staging_buffer_memory_index, 0, buffer_size_vertex, 0, &data_vertex);
  memcpy(data_vertex, fullscreen_quad->mesh->vertices->items, buffer_size_vertex);
  vkUnmapMemory(vulkan_renderer->device, staging_buffer_memory_index);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size_vertex, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &fullscreen_quad->vertex_buffer, &fullscreen_quad->vertex_buffer_memory);

  copy_buffer(vulkan_renderer, staging_buffer_vertex, fullscreen_quad->vertex_buffer, buffer_size_vertex);

  vkDestroyBuffer(vulkan_renderer->device, staging_buffer_vertex, NULL);
  vkFreeMemory(vulkan_renderer->device, staging_buffer_memory_index, NULL);

  // Index
  VkDeviceSize buffer_size_index = fullscreen_quad->mesh->indices->memory_size * fullscreen_quad->mesh->indices->size;
  VkBuffer staging_buffer_index_vertex = {0};
  VkDeviceMemory staging_buffer_memory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size_index, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer_index_vertex, &staging_buffer_memory);

  void* data_index;
  vkMapMemory(vulkan_renderer->device, staging_buffer_memory, 0, buffer_size_index, 0, &data_index);
  memcpy(data_index, fullscreen_quad->mesh->indices->items, buffer_size_index);
  vkUnmapMemory(vulkan_renderer->device, staging_buffer_memory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size_index, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &fullscreen_quad->index_buffer, &fullscreen_quad->index_buffer_memory);

  copy_buffer(vulkan_renderer, staging_buffer_index_vertex, fullscreen_quad->index_buffer, buffer_size_index);

  vkDestroyBuffer(vulkan_renderer->device, staging_buffer_index_vertex, NULL);
  vkFreeMemory(vulkan_renderer->device, staging_buffer_memory, NULL);
}

void fullscreen_quad_delete(struct FullscreenQuad* fullscreen_quad, struct VulkanRenderer* vulkan_renderer) {
  vkDestroyBuffer(vulkan_renderer->device, fullscreen_quad->index_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, fullscreen_quad->index_buffer_memory, NULL);

  vkDestroyBuffer(vulkan_renderer->device, fullscreen_quad->vertex_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, fullscreen_quad->vertex_buffer_memory, NULL);

  mesh_delete(fullscreen_quad->mesh);
  free(fullscreen_quad->mesh);
}
