#include "mana/graphics/dualcontouring/dualcontouring.h"

#define MAX_THRESHOLDS 5

int dual_contouring_init(struct DualContouring* dual_contouring, struct GPUAPI* gpu_api, int octree_size, struct Shader* shader, struct Vector* noises, float (*density_func_single)(struct Vector*, float, float, float), float* (*density_func_set)(struct Vector*, float, float, float, int, int, int)) {
  dual_contouring->noises = noises;
  dual_contouring->density_func_single = density_func_single;
  dual_contouring->density_func_set = density_func_set;
  dual_contouring->shader = shader;

  dual_contouring->mesh = calloc(1, sizeof(struct Mesh));
  mesh_dual_contouring_init(dual_contouring->mesh);

  dual_contouring->noise_set = density_func_set(noises, 0.0f, 0.0f, 0.0f, octree_size, octree_size, octree_size);

  const float THRESHOLDS[MAX_THRESHOLDS] = {-1.0f, 0.1f, 1.0f, 10.0f, 50.0f};
  int threshold_index = -1;
  threshold_index = (threshold_index + 1) % MAX_THRESHOLDS;
  dual_contouring->octree_size = octree_size;
  dual_contouring->head = octree_build_octree((ivec3){.data[0] = -dual_contouring->octree_size / 2, .data[1] = -dual_contouring->octree_size / 2, .data[2] = -dual_contouring->octree_size / 2}, dual_contouring->octree_size, THRESHOLDS[threshold_index], dual_contouring);
  octree_generate_mesh_from_octree(dual_contouring->head, dual_contouring);

  graphics_utils_setup_vertex_buffer(gpu_api->vulkan_state, dual_contouring->mesh->vertices, &dual_contouring->vertex_buffer, &dual_contouring->vertex_buffer_memory);
  graphics_utils_setup_index_buffer(gpu_api->vulkan_state, dual_contouring->mesh->indices, &dual_contouring->index_buffer, &dual_contouring->index_buffer_memory);
  graphics_utils_setup_uniform_buffer(gpu_api->vulkan_state, sizeof(struct DualContouringUniformBufferObject), &dual_contouring->dc_uniform_buffer, &dual_contouring->dc_uniform_buffer_memory);
  graphics_utils_setup_uniform_buffer(gpu_api->vulkan_state, sizeof(struct LightingUniformBufferObject), &dual_contouring->lighting_uniform_buffer, &dual_contouring->lighting_uniform_buffer_memory);
  graphics_utils_setup_descriptor(gpu_api->vulkan_state, dual_contouring->shader->descriptor_set_layout, dual_contouring->shader->descriptor_pool, &dual_contouring->descriptor_set);

  VkWriteDescriptorSet dcs[2] = {0};

  graphics_utils_setup_descriptor_buffer(gpu_api->vulkan_state, dcs, 0, &dual_contouring->descriptor_set, (VkDescriptorBufferInfo[]){graphics_utils_setup_descriptor_buffer_info(sizeof(struct DualContouringUniformBufferObject), &dual_contouring->dc_uniform_buffer)});
  graphics_utils_setup_descriptor_buffer(gpu_api->vulkan_state, dcs, 1, &dual_contouring->descriptor_set, (VkDescriptorBufferInfo[]){graphics_utils_setup_descriptor_buffer_info(sizeof(struct LightingUniformBufferObject), &dual_contouring->lighting_uniform_buffer)});

  vkUpdateDescriptorSets(gpu_api->vulkan_state->device, 2, dcs, 0, NULL);

  return 0;
}

static inline void dual_contouring_vulkan_cleanup(struct DualContouring* dual_contouring, struct GPUAPI* gpu_api) {
  vkDestroyBuffer(gpu_api->vulkan_state->device, dual_contouring->index_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, dual_contouring->index_buffer_memory, NULL);

  vkDestroyBuffer(gpu_api->vulkan_state->device, dual_contouring->vertex_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, dual_contouring->vertex_buffer_memory, NULL);

  vkDestroyBuffer(gpu_api->vulkan_state->device, dual_contouring->lighting_uniform_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, dual_contouring->lighting_uniform_buffer_memory, NULL);

  vkDestroyBuffer(gpu_api->vulkan_state->device, dual_contouring->dc_uniform_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, dual_contouring->dc_uniform_buffer_memory, NULL);
}

void dual_contouring_delete(struct DualContouring* dual_contouring, struct GPUAPI* gpu_api) {
  dual_contouring_vulkan_cleanup(dual_contouring, gpu_api);

  octree_destroy_octree(dual_contouring->head);
  mesh_delete(dual_contouring->mesh);
  free(dual_contouring->mesh);
  noise_free(dual_contouring->noise_set);
}

void dual_contouring_recreate(struct DualContouring* dual_contouring, struct GPUAPI* gpu_api) {
  dual_contouring_vulkan_cleanup(dual_contouring, gpu_api);

  graphics_utils_setup_vertex_buffer(gpu_api->vulkan_state, dual_contouring->mesh->vertices, &dual_contouring->vertex_buffer, &dual_contouring->vertex_buffer_memory);
  graphics_utils_setup_index_buffer(gpu_api->vulkan_state, dual_contouring->mesh->indices, &dual_contouring->index_buffer, &dual_contouring->index_buffer_memory);
  graphics_utils_setup_uniform_buffer(gpu_api->vulkan_state, sizeof(struct DualContouringUniformBufferObject), &dual_contouring->dc_uniform_buffer, &dual_contouring->dc_uniform_buffer_memory);
  graphics_utils_setup_uniform_buffer(gpu_api->vulkan_state, sizeof(struct LightingUniformBufferObject), &dual_contouring->lighting_uniform_buffer, &dual_contouring->lighting_uniform_buffer_memory);
  graphics_utils_setup_descriptor(gpu_api->vulkan_state, dual_contouring->shader->descriptor_set_layout, dual_contouring->shader->descriptor_pool, &dual_contouring->descriptor_set);

  VkWriteDescriptorSet dcs[2] = {0};

  graphics_utils_setup_descriptor_buffer(gpu_api->vulkan_state, dcs, 0, &dual_contouring->descriptor_set, (VkDescriptorBufferInfo[]){graphics_utils_setup_descriptor_buffer_info(sizeof(struct DualContouringUniformBufferObject), &dual_contouring->dc_uniform_buffer)});
  graphics_utils_setup_descriptor_buffer(gpu_api->vulkan_state, dcs, 1, &dual_contouring->descriptor_set, (VkDescriptorBufferInfo[]){graphics_utils_setup_descriptor_buffer_info(sizeof(struct LightingUniformBufferObject), &dual_contouring->lighting_uniform_buffer)});

  vkUpdateDescriptorSets(gpu_api->vulkan_state->device, 2, dcs, 0, NULL);
}
