#include "mana/graphics/dualcontouring/dualcontouring.h"

#define MAX_THRESHOLDS 5

int dual_contouring_init(struct DualContouring* dual_contouring, struct VulkanRenderer* vulkan_renderer, int octree_size, struct Shader* shader) {
  dual_contouring->mesh = calloc(1, sizeof(struct Mesh));
  mesh_init(dual_contouring->mesh);

  const float THRESHOLDS[MAX_THRESHOLDS] = {-1.0f, 0.1f, 1.0f, 10.0f, 50.0f};
  int threshold_index = -1;
  threshold_index = (threshold_index + 1) % MAX_THRESHOLDS;
  dual_contouring->octree_size = octree_size;
  dual_contouring->head = octree_build_octree((ivec3){-dual_contouring->octree_size / 2, -dual_contouring->octree_size / 2, -dual_contouring->octree_size / 2}, dual_contouring->octree_size, THRESHOLDS[threshold_index]);
  octree_generate_mesh_from_octree(dual_contouring->head, dual_contouring->mesh);

  // Vertex buffer
  VkDeviceSize vertex_buffer_size = dual_contouring->mesh->vertices->memory_size * dual_contouring->mesh->vertices->size;
  VkBuffer vertex_staging_buffer = {0};
  VkDeviceMemory vertex_staging_buffer_memory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, vertex_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vertex_staging_buffer, &vertex_staging_buffer_memory);

  void* vertex_data;
  vkMapMemory(vulkan_renderer->device, vertex_staging_buffer_memory, 0, vertex_buffer_size, 0, &vertex_data);
  memcpy(vertex_data, dual_contouring->mesh->vertices->items, vertex_buffer_size);
  vkUnmapMemory(vulkan_renderer->device, vertex_staging_buffer_memory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, vertex_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &dual_contouring->vertex_buffer, &dual_contouring->vertex_buffer_memory);

  copy_buffer(vulkan_renderer, vertex_staging_buffer, dual_contouring->vertex_buffer, vertex_buffer_size);

  vkDestroyBuffer(vulkan_renderer->device, vertex_staging_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, vertex_staging_buffer_memory, NULL);

  // Index buffer
  VkDeviceSize index_buffer_size = dual_contouring->mesh->indices->memory_size * dual_contouring->mesh->indices->size;
  VkBuffer index_staging_buffer = {0};
  VkDeviceMemory index_staging_buffer_memory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, index_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &index_staging_buffer, &index_staging_buffer_memory);

  void* index_data;
  vkMapMemory(vulkan_renderer->device, index_staging_buffer_memory, 0, index_buffer_size, 0, &index_data);
  memcpy(index_data, dual_contouring->mesh->indices->items, index_buffer_size);
  vkUnmapMemory(vulkan_renderer->device, index_staging_buffer_memory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, index_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &dual_contouring->index_buffer, &dual_contouring->index_buffer_memory);

  copy_buffer(vulkan_renderer, index_staging_buffer, dual_contouring->index_buffer, index_buffer_size);

  vkDestroyBuffer(vulkan_renderer->device, index_staging_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, index_staging_buffer_memory, NULL);

  // Uniform buffer
  VkDeviceSize uniform_buffer_size = sizeof(struct DualContouringUniformBufferObject);
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, uniform_buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &dual_contouring->uniform_buffer, &dual_contouring->uniform_buffers_memory);

  // Descriptor sets
  VkDescriptorSetLayout layout = {0};
  layout = shader->descriptor_set_layout;

  VkDescriptorSetAllocateInfo alloc_info = {0};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = shader->descriptor_pool;
  alloc_info.descriptorSetCount = 1;
  alloc_info.pSetLayouts = &layout;

  if (vkAllocateDescriptorSets(vulkan_renderer->device, &alloc_info, &dual_contouring->descriptor_set) != VK_SUCCESS) {
    fprintf(stderr, "failed to allocate descriptor sets!\n");
    return 0;
  }

  VkDescriptorBufferInfo buffer_info = {0};
  buffer_info.buffer = dual_contouring->uniform_buffer;
  buffer_info.offset = 0;
  buffer_info.range = sizeof(struct DualContouringUniformBufferObject);

  VkWriteDescriptorSet dc = {0};

  dc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  dc.dstSet = dual_contouring->descriptor_set;
  dc.dstBinding = 0;
  dc.dstArrayElement = 0;
  dc.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  dc.descriptorCount = 1;
  dc.pBufferInfo = &buffer_info;

  vkUpdateDescriptorSets(vulkan_renderer->device, 1, &dc, 0, NULL);

  return 0;
}

void dual_contouring_delete(struct DualContouring* dual_contouring, struct VulkanRenderer* vulkan_renderer) {
  vkDestroyBuffer(vulkan_renderer->device, dual_contouring->index_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, dual_contouring->index_buffer_memory, NULL);

  vkDestroyBuffer(vulkan_renderer->device, dual_contouring->vertex_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, dual_contouring->vertex_buffer_memory, NULL);

  vkDestroyBuffer(vulkan_renderer->device, dual_contouring->uniform_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, dual_contouring->uniform_buffers_memory, NULL);

  octree_destroy_octree(dual_contouring->head);
  mesh_delete(dual_contouring->mesh);
  free(dual_contouring->mesh);
}