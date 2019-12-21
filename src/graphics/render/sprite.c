#include "graphics/render/sprite.h"

int sprite_init(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  sprite->image_mesh = calloc(1, sizeof(struct Mesh));
  mesh_init(sprite->image_mesh);

  sprite->image_texture = calloc(1, sizeof(struct Texture));
  texture_init(sprite->image_texture, "./Assets/textures/texture.jpg");

  mesh_init(sprite->image_mesh);

  mesh_assign_vertex(sprite->image_mesh->vertices, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  mesh_assign_vertex(sprite->image_mesh->vertices, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
  mesh_assign_vertex(sprite->image_mesh->vertices, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
  mesh_assign_vertex(sprite->image_mesh->vertices, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

  mesh_assign_indice(sprite->image_mesh->indices, 0);
  mesh_assign_indice(sprite->image_mesh->indices, 1);
  mesh_assign_indice(sprite->image_mesh->indices, 2);
  mesh_assign_indice(sprite->image_mesh->indices, 2);
  mesh_assign_indice(sprite->image_mesh->indices, 3);
  mesh_assign_indice(sprite->image_mesh->indices, 0);

  //int error_code;

  texture_create_image(vulkan_renderer, sprite->image_texture);
  texture_create_texture_image_view(vulkan_renderer, sprite->image_texture);
  texture_create_sampler(vulkan_renderer, sprite->image_texture);

  sprite_create_vertex_buffer(sprite, vulkan_renderer);
  sprite_create_index_buffer(sprite, vulkan_renderer);
  sprite_create_uniform_buffers(sprite, vulkan_renderer);
  //sprite_create_descriptor_sets(sprite, vulkan_renderer);

  return SPRITE_SUCCESS;

  /*vulkan_index_buffer_error:
  vulkan_index_buffer_cleanup(vulkan_renderer);
vulkan_vertex_buffer_error:
  vulkan_vertex_buffer_cleanup(vulkan_renderer);
vulkan_texture_error:
  vulkan_texture_cleanup(vulkan_renderer);
  //
  for (int buffer_num = 0; buffer_num < MAX_SWAP_CHAIN_FRAMES; buffer_num++) {
    vkDestroyBuffer(vulkan_renderer->device, vulkan_renderer->uniform_buffers[buffer_num], NULL);
    vkFreeMemory(vulkan_renderer->device, vulkan_renderer->uniform_buffers_memory[buffer_num], NULL);
  }
  //
vulkan_desriptor_set_layout_error:
  vulkan_descriptor_set_layout_cleanup(vulkan_renderer);*/
}

void sprite_delete(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  vulkan_index_buffer_cleanup(sprite, vulkan_renderer);
  vulkan_vertex_buffer_cleanup(sprite, vulkan_renderer);
  for (int buffer_num = 0; buffer_num < MAX_SWAP_CHAIN_FRAMES; buffer_num++) {
    vkDestroyBuffer(vulkan_renderer->device, sprite->uniform_buffers[buffer_num], NULL);
    vkFreeMemory(vulkan_renderer->device, sprite->uniform_buffers_memory[buffer_num], NULL);
  }
  mesh_delete(sprite->image_mesh);
  free(sprite->image_mesh);
  vulkan_texture_cleanup(sprite, vulkan_renderer);
}

int sprite_create_vertex_buffer(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  VkDeviceSize buffer_size = sprite->image_mesh->vertices->memory_size * sprite->image_mesh->vertices->size;
  //VkDeviceSize bufferSize = sizeof(vulkan_renderer->imageVertices.items[0]) * vulkan_renderer->imageVertices.total;

  VkBuffer staging_buffer = {0};
  VkDeviceMemory stagingBufferMemory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer, &stagingBufferMemory);

  void* data;
  vkMapMemory(vulkan_renderer->device, stagingBufferMemory, 0, buffer_size, 0, &data);
  memcpy(data, sprite->image_mesh->vertices->items, buffer_size);
  vkUnmapMemory(vulkan_renderer->device, stagingBufferMemory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &sprite->vertex_buffer, &sprite->vertex_buffer_memory);

  copy_buffer(vulkan_renderer, staging_buffer, sprite->vertex_buffer, buffer_size);

  vkDestroyBuffer(vulkan_renderer->device, staging_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, stagingBufferMemory, NULL);

  return 0;
}

int sprite_create_index_buffer(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  VkDeviceSize buffer_size = sprite->image_mesh->indices->memory_size * sprite->image_mesh->indices->size;
  //VkDeviceSize bufferSize = sizeof(vulkan_renderer->imageIndices.items[0]) * vulkan_renderer->imageIndices.total;

  VkBuffer staging_buffer = {0};
  VkDeviceMemory staging_buffer_memory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer, &staging_buffer_memory);

  void* data;
  vkMapMemory(vulkan_renderer->device, staging_buffer_memory, 0, buffer_size, 0, &data);
  memcpy(data, sprite->image_mesh->indices->items, buffer_size);
  vkUnmapMemory(vulkan_renderer->device, staging_buffer_memory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &sprite->index_buffer, &sprite->index_buffer_memory);

  copy_buffer(vulkan_renderer, staging_buffer, sprite->index_buffer, buffer_size);

  vkDestroyBuffer(vulkan_renderer->device, staging_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, staging_buffer_memory, NULL);

  return 0;
}

int sprite_create_uniform_buffers(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  VkDeviceSize buffer_size = sizeof(struct UniformBufferObject);

  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++)
    graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &sprite->uniform_buffers[i], &sprite->uniform_buffers_memory[i]);

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////
void vulkan_index_buffer_cleanup(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  vkDestroyBuffer(vulkan_renderer->device, sprite->index_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, sprite->index_buffer_memory, NULL);
}

void vulkan_vertex_buffer_cleanup(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  vkDestroyBuffer(vulkan_renderer->device, sprite->vertex_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, sprite->vertex_buffer_memory, NULL);
}

void vulkan_texture_cleanup(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  texture_delete(vulkan_renderer, sprite->image_texture);
  free(sprite->image_texture);
}
