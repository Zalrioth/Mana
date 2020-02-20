#include "mana/graphics/dualcontouring/dualcontouring.h"

#define MAX_THRESHOLDS 5

int HelloThread(void* aArg) {
  printf("Hello world!\n");
  return 0;
}

int dual_contouring_init(struct DualContouring* dual_contouring, struct VulkanRenderer* vulkan_renderer, int octree_size, struct Shader* shader, struct Vector* noises, float (*density_func_single)(struct Vector*, float, float, float), float* (*density_func_set)(struct Vector*, float, float, float, int, int, int)) {
  dual_contouring->noises = noises;
  dual_contouring->density_func_single = density_func_single;
  dual_contouring->density_func_set = density_func_set;

  dual_contouring->mesh = calloc(1, sizeof(struct Mesh));
  mesh_init(dual_contouring->mesh);

  struct Map map = {0};
  map_init(&map, sizeof(int));

  int value = 123;

  map_set(&map, "testkey1", &value);
  map_set(&map, "testkey2", &value);
  map_set(&map, "testkey3", &value);
  map_set(&map, "testkey4", &value);
  int* val = map_get(&map, "testkey3");
  val ? printf("value: %d\n", *val) : printf("value not found\n");
  const char* key;
  struct MapIter iter = map_iter();
  while ((key = map_next(&map, &iter)))
    printf("%s -> %d\n", key, *(int*)map_get(&map, key));
  map_delete(&map);

  dual_contouring->noise_set = density_func_set(noises, 0.0f, 0.0f, 0.0f, octree_size, octree_size, octree_size);

  //thrd_t t;
  //if (thrd_create(&t, HelloThread, NULL) == thrd_success)
  //  thrd_join(t, NULL);

  const float THRESHOLDS[MAX_THRESHOLDS] = {-1.0f, 0.1f, 1.0f, 10.0f, 50.0f};
  int threshold_index = -1;
  threshold_index = (threshold_index + 1) % MAX_THRESHOLDS;
  dual_contouring->octree_size = octree_size;
  dual_contouring->head = octree_build_octree((ivec3){-dual_contouring->octree_size / 2, -dual_contouring->octree_size / 2, -dual_contouring->octree_size / 2}, dual_contouring->octree_size, THRESHOLDS[threshold_index], dual_contouring);
  octree_generate_mesh_from_octree(dual_contouring->head, dual_contouring);

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

  // Uniform buffers
  VkDeviceSize dc_uniform_buffer_size = sizeof(struct DualContouringUniformBufferObject);
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, dc_uniform_buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &dual_contouring->dc_uniform_buffer, &dual_contouring->dc_uniform_buffer_memory);

  VkDeviceSize lighting_uniform_buffer_size = sizeof(struct LightingUniformBufferObject);
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, lighting_uniform_buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &dual_contouring->lighting_uniform_buffer, &dual_contouring->lighting_uniform_buffer_memory);

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

  VkDescriptorBufferInfo dc_buffer_info = {0};
  dc_buffer_info.buffer = dual_contouring->dc_uniform_buffer;
  dc_buffer_info.offset = 0;
  dc_buffer_info.range = sizeof(struct DualContouringUniformBufferObject);

  VkWriteDescriptorSet dc = {0};

  dc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  dc.dstSet = dual_contouring->descriptor_set;
  dc.dstBinding = 0;
  dc.dstArrayElement = 0;
  dc.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  dc.descriptorCount = 1;
  dc.pBufferInfo = &dc_buffer_info;

  vkUpdateDescriptorSets(vulkan_renderer->device, 1, &dc, 0, NULL);

  VkDescriptorBufferInfo lighting_buffer_info = {0};
  lighting_buffer_info.buffer = dual_contouring->lighting_uniform_buffer;
  lighting_buffer_info.offset = 0;
  lighting_buffer_info.range = sizeof(struct LightingUniformBufferObject);

  VkWriteDescriptorSet lighting = {0};

  lighting.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  lighting.dstSet = dual_contouring->descriptor_set;
  lighting.dstBinding = 1;
  lighting.dstArrayElement = 0;
  lighting.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  lighting.descriptorCount = 1;
  lighting.pBufferInfo = &lighting_buffer_info;

  vkUpdateDescriptorSets(vulkan_renderer->device, 1, &lighting, 0, NULL);

  return 0;
}

void dual_contouring_delete(struct DualContouring* dual_contouring, struct VulkanRenderer* vulkan_renderer) {
  vkDestroyBuffer(vulkan_renderer->device, dual_contouring->index_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, dual_contouring->index_buffer_memory, NULL);

  vkDestroyBuffer(vulkan_renderer->device, dual_contouring->vertex_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, dual_contouring->vertex_buffer_memory, NULL);

  vkDestroyBuffer(vulkan_renderer->device, dual_contouring->lighting_uniform_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, dual_contouring->lighting_uniform_buffer_memory, NULL);

  vkDestroyBuffer(vulkan_renderer->device, dual_contouring->dc_uniform_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, dual_contouring->dc_uniform_buffer_memory, NULL);

  octree_destroy_octree(dual_contouring->head);
  mesh_delete(dual_contouring->mesh);
  free(dual_contouring->mesh);
}