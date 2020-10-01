#include "mana/graphics/dualcontouring/manifold/manifolddualcontouring.h"

//: base(device, resolution, size, true, !FlatShading, 2097152)
void manifold_dual_contouring_init(struct ManifoldDualContouring* manifold_dual_contouring, struct GPUAPI* gpu_api, struct Shader* shader, int resolution, int size) {
  manifold_dual_contouring->shader = shader;

  manifold_dual_contouring->mesh = calloc(1, sizeof(struct Mesh));
  mesh_manifold_dual_contouring_init(manifold_dual_contouring->mesh);

  manifold_dual_contouring->octree_size = size;
  manifold_dual_contouring->resolution = resolution;
}

static inline void manifold_dual_contouring_vulkan_cleanup(struct ManifoldDualContouring* manifold_dual_contouring, struct GPUAPI* gpu_api) {
  vkDestroyBuffer(gpu_api->vulkan_state->device, manifold_dual_contouring->index_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, manifold_dual_contouring->index_buffer_memory, NULL);

  vkDestroyBuffer(gpu_api->vulkan_state->device, manifold_dual_contouring->vertex_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, manifold_dual_contouring->vertex_buffer_memory, NULL);

  vkDestroyBuffer(gpu_api->vulkan_state->device, manifold_dual_contouring->lighting_uniform_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, manifold_dual_contouring->lighting_uniform_buffer_memory, NULL);

  vkDestroyBuffer(gpu_api->vulkan_state->device, manifold_dual_contouring->dc_uniform_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, manifold_dual_contouring->dc_uniform_buffer_memory, NULL);
}

void manifold_dual_contouring_delete(struct ManifoldDualContouring* manifold_dual_contouring, struct GPUAPI* gpu_api) {
  manifold_dual_contouring_vulkan_cleanup(manifold_dual_contouring, gpu_api);

  // Search through octree for unique vertices
  struct Map vertice_map = {0};
  map_init(&vertice_map, sizeof(struct Vertex*));
  manifold_octree_destroy_octree(manifold_dual_contouring->tree, &vertice_map);
  const char* key;
  struct MapIter iter = map_iter();
  while ((key = map_next(&vertice_map, &iter)))
    free(*(char**)map_get(&vertice_map, key));
  map_delete(&vertice_map);

  mesh_delete(manifold_dual_contouring->mesh);
  free(manifold_dual_contouring->mesh);
  //noise_free(manifold_dual_contouring->noise_set);
}

static inline void manifold_dual_contouring_setup_buffers(struct ManifoldDualContouring* manifold_dual_contouring, struct GPUAPI* gpu_api) {
  graphics_utils_setup_vertex_buffer(gpu_api->vulkan_state, manifold_dual_contouring->mesh->vertices, &manifold_dual_contouring->vertex_buffer, &manifold_dual_contouring->vertex_buffer_memory);
  graphics_utils_setup_index_buffer(gpu_api->vulkan_state, manifold_dual_contouring->mesh->indices, &manifold_dual_contouring->index_buffer, &manifold_dual_contouring->index_buffer_memory);
  graphics_utils_setup_uniform_buffer(gpu_api->vulkan_state, sizeof(struct ManifoldDualContouringUniformBufferObject), &manifold_dual_contouring->dc_uniform_buffer, &manifold_dual_contouring->dc_uniform_buffer_memory);
  graphics_utils_setup_uniform_buffer(gpu_api->vulkan_state, sizeof(struct LightingUniformBufferObject), &manifold_dual_contouring->lighting_uniform_buffer, &manifold_dual_contouring->lighting_uniform_buffer_memory);
  graphics_utils_setup_descriptor(gpu_api->vulkan_state, manifold_dual_contouring->shader->descriptor_set_layout, manifold_dual_contouring->shader->descriptor_pool, &manifold_dual_contouring->descriptor_set);

  VkWriteDescriptorSet dcs[2] = {0};

  graphics_utils_setup_descriptor_buffer(gpu_api->vulkan_state, dcs, 0, &manifold_dual_contouring->descriptor_set, (VkDescriptorBufferInfo[]){graphics_utils_setup_descriptor_buffer_info(sizeof(struct ManifoldDualContouringUniformBufferObject), &manifold_dual_contouring->dc_uniform_buffer)});
  graphics_utils_setup_descriptor_buffer(gpu_api->vulkan_state, dcs, 1, &manifold_dual_contouring->descriptor_set, (VkDescriptorBufferInfo[]){graphics_utils_setup_descriptor_buffer_info(sizeof(struct LightingUniformBufferObject), &manifold_dual_contouring->lighting_uniform_buffer)});

  vkUpdateDescriptorSets(gpu_api->vulkan_state->device, 2, dcs, 0, NULL);
}

void manifold_dual_contouring_recreate(struct ManifoldDualContouring* manifold_dual_contouring, struct GPUAPI* gpu_api) {
  manifold_dual_contouring_vulkan_cleanup(manifold_dual_contouring, gpu_api);
  manifold_dual_contouring_setup_buffers(manifold_dual_contouring, gpu_api);
}

void manifold_dual_contouring_contour(struct ManifoldDualContouring* manifold_dual_contouring, struct GPUAPI* gpu_api, float threshold) {
  mesh_clear(manifold_dual_contouring->mesh);
  manifold_dual_contouring->tree = calloc(1, sizeof(struct ManifoldOctreeNode));

#if MANIFOLD_BENCHMARK
  double start_time = engine_get_time();
  manifold_octree_construct_base(manifold_dual_contouring->tree, manifold_dual_contouring->resolution, 0);
  double end_time = engine_get_time();
  printf("Total time taken: %lf\n", end_time - start_time);
#else
  manifold_octree_construct_base(manifold_dual_contouring->tree, manifold_dual_contouring->resolution, 0);
#endif
  manifold_octree_cluster_cell_base(manifold_dual_contouring->tree, 0);

  manifold_octree_generate_vertex_buffer(manifold_dual_contouring->tree, manifold_dual_contouring->mesh->vertices);
  manifold_octree_process_cell(manifold_dual_contouring->tree, manifold_dual_contouring->mesh->indices, threshold);

  if (vector_size(manifold_dual_contouring->mesh->vertices) > 0)
    manifold_dual_contouring_setup_buffers(manifold_dual_contouring, gpu_api);
}

//void manifold_dual_contouring_construct_tree_grid(struct ManifoldOctreeNode* node) {
//  if (node == NULL)
//    return;
//
//  int x = (int)node->position.x;
//  int y = (int)node->position.y;
//  int z = (int)node->position.z;
//  vec3 c = {.r = 0.0f, .g = 0.2f, .b = 0.8f};
//  vec3 v = {.r = 1.0f, .g = 0.0f, .b = 0.0f};
//
//  float size = node->size;
//
//  if (node->type == MANIFOLD_NODE_INTERNAL && array_list_size(node->vertices) == 0) {
//    for (int i = 0; i < 8; i++) {
//      manifold_dual_contouring_construct_tree_grid(node->children[i]);
//    }
//  }
//}
