#include "mana/graphics/dualcontouring/manifold/manifolddualcontouring.h"

//: base(device, resolution, size, true, !FlatShading, 2097152)
void manifold_dual_contouring_init(struct ManifoldDualContouring* manifold_dual_contouring, struct GPUAPI* gpu_api, struct Shader* shader, int resolution, int size) {
  manifold_dual_contouring->shader = shader;

  manifold_dual_contouring->mesh = calloc(1, sizeof(struct Mesh));
  mesh_manifold_dual_contouring_init(manifold_dual_contouring->mesh);

  manifold_dual_contouring->octree_size = size;
  manifold_dual_contouring->resolution = resolution;
}

void manifold_dual_contouring_contour(struct ManifoldDualContouring* manifold_dual_contouring, struct GPUAPI* gpu_api, float threshold) {
  vector_clear(manifold_dual_contouring->mesh->vertices);
  manifold_dual_contouring->tree = calloc(1, sizeof(struct ManifoldOctreeNode));

  manifold_octree_construct_base(manifold_dual_contouring->tree, 64, 0, manifold_dual_contouring->mesh->vertices);
  manifold_octree_cluster_cell_base(manifold_dual_contouring->tree, 0);

  manifold_octree_generate_vertex_buffer(manifold_dual_contouring->tree, manifold_dual_contouring->mesh->vertices);
  manifold_dual_contouring_calculate_indexes(manifold_dual_contouring, threshold);

  if (vector_size(manifold_dual_contouring->mesh->vertices) > 0) {
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
}

void manifold_dual_contouring_construct_tree_grid(struct ManifoldOctreeNode* node) {
  if (node == NULL)
    return;

  int x = (int)node->position.x;
  int y = (int)node->position.y;
  int z = (int)node->position.z;
  vec3 c = {.r = 0.0f, .g = 0.2f, .b = 0.8f};
  vec3 v = {.r = 1.0f, .g = 0.0f, .b = 0.0f};

  float size = node->size;

  if (node->type == MANIFOLD_NODE_INTERNAL && vector_size(node->vertices) == 0) {
    for (int i = 0; i < 8; i++) {
      manifold_dual_contouring_construct_tree_grid(node->children[i]);
    }
  }
}

void manifold_dual_contouring_calculate_indexes(struct ManifoldDualContouring* manifold_dual_contouring, float threshold) {
  //vector_clear(manifold_dual_contouring->mesh->indices);
  struct Vector tri_count;
  vector_init(&tri_count, sizeof(int));

  manifold_octree_process_cell(manifold_dual_contouring->tree, manifold_dual_contouring->mesh->indices, &tri_count, threshold);

  int t_index = 0;
  for (int i = 0; i < vector_size(manifold_dual_contouring->mesh->indices); i += 3) {
    int count = *((int*)vector_get(&tri_count, t_index++));
    vec3 n = VEC3_ZERO;
    int indexes_raw[6] = {*((int*)vector_get(manifold_dual_contouring->mesh->indices, i + 0)) & 0xFFFFFFF,
                          *((int*)vector_get(manifold_dual_contouring->mesh->indices, i + 1)) & 0xFFFFFFF,
                          *((int*)vector_get(manifold_dual_contouring->mesh->indices, i + 2)) & 0xFFFFFFF,
                          *((int*)vector_get(manifold_dual_contouring->mesh->indices, i + 3)) & 0xFFFFFFF,
                          *((int*)vector_get(manifold_dual_contouring->mesh->indices, i + 4)) & 0xFFFFFFF,
                          *((int*)vector_get(manifold_dual_contouring->mesh->indices, i + 5)) & 0xFFFFFFF};
    if (count == 1) {
      int size = 3;
      int indexes[6] = {indexes_raw[2], indexes_raw[0], indexes_raw[1], 0, 0, 0};
      n = manifold_dual_contouring_get_normal_q(manifold_dual_contouring->mesh->vertices, indexes, size);
    } else {
      int size = 6;
      int indexes[6] = {indexes_raw[2], indexes_raw[0], indexes_raw[1],
                        indexes_raw[5], indexes_raw[3], indexes_raw[4]};
      n = manifold_dual_contouring_get_normal_q(manifold_dual_contouring->mesh->vertices, indexes, size);
    }
    vec3 nc = vec3_add(vec3_scale(n, 0.5f), vec3_scale(VEC3_ONE, 0.5f));
    nc = vec3_normalise(nc);
    vec3 c = nc;

    struct VertexManifoldDualContouring* vertex_stuff[6] = {(struct VertexManifoldDualContouring*)vector_get(manifold_dual_contouring->mesh->vertices, indexes_raw[0]),
                                                            (struct VertexManifoldDualContouring*)vector_get(manifold_dual_contouring->mesh->vertices, indexes_raw[1]),
                                                            (struct VertexManifoldDualContouring*)vector_get(manifold_dual_contouring->mesh->vertices, indexes_raw[2]),
                                                            (struct VertexManifoldDualContouring*)vector_get(manifold_dual_contouring->mesh->vertices, indexes_raw[3]),
                                                            (struct VertexManifoldDualContouring*)vector_get(manifold_dual_contouring->mesh->vertices, indexes_raw[4]),
                                                            (struct VertexManifoldDualContouring*)vector_get(manifold_dual_contouring->mesh->vertices, indexes_raw[5])};

    struct VertexManifoldDualContouring v0 = (struct VertexManifoldDualContouring){vertex_stuff[0]->position, c, n, vertex_stuff[0]->normal1};
    struct VertexManifoldDualContouring v1 = (struct VertexManifoldDualContouring){vertex_stuff[1]->position, c, n, vertex_stuff[1]->normal1};
    struct VertexManifoldDualContouring v2 = (struct VertexManifoldDualContouring){vertex_stuff[2]->position, c, n, vertex_stuff[2]->normal1};

    vector_push_back(manifold_dual_contouring->mesh->vertices, &v0);
    vector_push_back(manifold_dual_contouring->mesh->vertices, &v1);
    vector_push_back(manifold_dual_contouring->mesh->vertices, &v2);

    if (count > 1) {
      struct VertexManifoldDualContouring v3 = (struct VertexManifoldDualContouring){vertex_stuff[3]->position, c, n, vertex_stuff[3]->normal1};
      struct VertexManifoldDualContouring v4 = (struct VertexManifoldDualContouring){vertex_stuff[4]->position, c, n, vertex_stuff[4]->normal1};
      struct VertexManifoldDualContouring v5 = (struct VertexManifoldDualContouring){vertex_stuff[5]->position, c, n, vertex_stuff[5]->normal1};

      vector_push_back(manifold_dual_contouring->mesh->vertices, &v3);
      vector_push_back(manifold_dual_contouring->mesh->vertices, &v4);
      vector_push_back(manifold_dual_contouring->mesh->vertices, &v5);

      i += 3;
    }
  }
}

vec3 manifold_dual_contouring_get_normal_q(struct Vector* verts, int indexes[6], int index_length) {
  vec3 a = vec3_sub(((struct VertexManifoldDualContouring*)vector_get(verts, indexes[2]))->position, ((struct VertexManifoldDualContouring*)vector_get(verts, indexes[1]))->position);
  vec3 b = vec3_sub(((struct VertexManifoldDualContouring*)vector_get(verts, indexes[2]))->position, ((struct VertexManifoldDualContouring*)vector_get(verts, indexes[0]))->position);
  vec3 c = vec3_cross_product(a, b);

  if (index_length == 6) {
    a = vec3_sub(((struct VertexManifoldDualContouring*)vector_get(verts, indexes[5]))->position, ((struct VertexManifoldDualContouring*)vector_get(verts, indexes[4]))->position);
    b = vec3_sub(((struct VertexManifoldDualContouring*)vector_get(verts, indexes[5]))->position, ((struct VertexManifoldDualContouring*)vector_get(verts, indexes[3]))->position);
    vec3 d = vec3_cross_product(a, b);

    if (c.x == NAN)
      c = VEC3_ZERO;
    if (d.x == NAN)
      d = VEC3_ZERO;

    c = vec3_add(c, d);
    c = vec3_divs(c, 2.0f);
  }

  c = vec3_normalise(c);

  return vec3_invert(c);
}
