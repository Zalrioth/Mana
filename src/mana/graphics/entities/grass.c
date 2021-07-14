#include "mana/graphics/entities/grass.h"

int grass_init(struct Grass* grass, struct GPUAPI* gpu_api) {
  grass->mesh = calloc(1, sizeof(struct Mesh));
  mesh_grass_init(grass->mesh);

  //mesh_grass_assign_vertex(grass->mesh->vertices, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
  //mesh_grass_assign_vertex(grass->mesh->vertices, -1.0f, -2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
  //mesh_grass_assign_vertex(grass->mesh->vertices, 2.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
  //
  //mesh_assign_indice(grass->mesh->indices, 0);
  //mesh_assign_indice(grass->mesh->indices, 1);
  //mesh_assign_indice(grass->mesh->indices, 2);

  graphics_utils_setup_vertex_buffer_pool(gpu_api->vulkan_state, grass->mesh->vertices, 1024, &grass->vertex_buffer, &grass->vertex_buffer_memory);
  graphics_utils_setup_index_buffer_pool(gpu_api->vulkan_state, grass->mesh->indices, 1024, &grass->index_buffer, &grass->index_buffer_memory);
  graphics_utils_setup_uniform_buffer(gpu_api->vulkan_state, sizeof(struct GrassUniformBufferObject), &grass->uniform_buffer, &grass->uniform_buffers_memory);

  grass_shader_init(&grass->grass_shader, gpu_api);

  graphics_utils_setup_descriptor(gpu_api->vulkan_state, grass->grass_shader.grass_render_shader.descriptor_set_layout, grass->grass_shader.grass_render_shader.descriptor_pool, &grass->descriptor_set);

  VkWriteDescriptorSet dcs[1] = {0};
  graphics_utils_setup_descriptor_buffer(gpu_api->vulkan_state, dcs, 0, &grass->descriptor_set, (VkDescriptorBufferInfo[]){graphics_utils_setup_descriptor_buffer_info(sizeof(struct GrassUniformBufferObject), &grass->uniform_buffer)});
  vkUpdateDescriptorSets(gpu_api->vulkan_state->device, 1, dcs, 0, NULL);

  vector_init(&grass->grass_nodes, sizeof(vec3));

  vec3 grass1 = {0.0f, 0.0f, 0.0f};
  vector_push_back(&grass->grass_nodes, &grass1);

  return 1;
}

static inline void grass_vulkan_cleanup(struct Grass* grass, struct GPUAPI* gpu_api) {
  vkDestroyBuffer(gpu_api->vulkan_state->device, grass->index_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, grass->index_buffer_memory, NULL);

  vkDestroyBuffer(gpu_api->vulkan_state->device, grass->vertex_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, grass->vertex_buffer_memory, NULL);

  vkDestroyBuffer(gpu_api->vulkan_state->device, grass->uniform_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, grass->uniform_buffers_memory, NULL);
}

void grass_delete(struct Grass* grass, struct VulkanState* vulkan_state) {
  grass_shader_delete(&grass->grass_shader, vulkan_state);

  vkDestroyBuffer(vulkan_state->device, grass->index_buffer, NULL);
  vkFreeMemory(vulkan_state->device, grass->index_buffer_memory, NULL);

  vkDestroyBuffer(vulkan_state->device, grass->vertex_buffer, NULL);
  vkFreeMemory(vulkan_state->device, grass->vertex_buffer_memory, NULL);

  mesh_delete(grass->mesh);
  free(grass->mesh);
}

struct in_grass_vertices {
  int total_grass_vertices;
  vec3 grass_vertices[1024];
};

struct out_draw_grass_vertices {
  unsigned int total_draw_grass_vertices;
  vec3 draw_grass_vertices[1024];
};
struct out_draw_grass_indices {
  unsigned int total_draw_grass_indices;
  unsigned int draw_grass_indices[1024];
};

void grass_render(struct Grass* grass, struct GPUAPI* gpu_api) {
  // TODO: Pass grass vertice points into compute shader
  // Copy compute shader vertice and indice output into
  const uint32_t elements = 1024;
  VkCommandBufferBeginInfo beginInfo = {0};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(grass->grass_shader.commandBuffer, &beginInfo);
  vkCmdBindPipeline(grass->grass_shader.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, grass->grass_shader.grass_compute_shader.graphics_pipeline);
  vkCmdBindDescriptorSets(grass->grass_shader.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, grass->grass_shader.grass_compute_shader.pipeline_layout, 0, 1, &grass->grass_shader.descriptorSet, 0, NULL);
  vkCmdDispatch(grass->grass_shader.commandBuffer, elements, 1, 1);
  vkEndCommandBuffer(grass->grass_shader.commandBuffer);

  void* data1 = NULL;
  vkMapMemory(gpu_api->vulkan_state->device, grass->grass_shader.grass_compute_memory[0], 0, VK_WHOLE_SIZE, 0, &data1);

  void* data2 = NULL;
  vkMapMemory(gpu_api->vulkan_state->device, grass->grass_shader.grass_compute_memory[1], 0, VK_WHOLE_SIZE, 0, &data2);

  void* data3 = NULL;
  vkMapMemory(gpu_api->vulkan_state->device, grass->grass_shader.grass_compute_memory[2], 0, VK_WHOLE_SIZE, 0, &data3);

  struct in_grass_vertices* d_a = data1;
  struct out_draw_grass_vertices* d_b = data2;
  struct out_draw_grass_indices* d_c = data3;

  //d_a->total_grass_vertices = 1;
  //vec3 pos = {1.0, 1.0, 1.0};
  //d_a->grass_vertices[0] = pos;
  //for (uint32_t i = 0; i < elements; i++) {
  //  d_a[i] = dis(gen);
  //  d_b[i] = dis(gen);
  //  d_c[i] = 0.0;
  //}

  vkUnmapMemory(gpu_api->vulkan_state->device, grass->grass_shader.grass_compute_memory[0]);
  vkUnmapMemory(gpu_api->vulkan_state->device, grass->grass_shader.grass_compute_memory[1]);
  vkUnmapMemory(gpu_api->vulkan_state->device, grass->grass_shader.grass_compute_memory[2]);

  // Return stuff
  VkSubmitInfo submitInfo = {0};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &grass->grass_shader.commandBuffer;
  vkQueueSubmit(gpu_api->vulkan_state->graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
  // but we can simply wait for all the work to be done
  vkQueueWaitIdle(gpu_api->vulkan_state->graphics_queue);

  data1 = NULL;
  vkMapMemory(gpu_api->vulkan_state->device, grass->grass_shader.grass_compute_memory[0], 0, VK_WHOLE_SIZE, 0, &data1);

  data2 = NULL;
  vkMapMemory(gpu_api->vulkan_state->device, grass->grass_shader.grass_compute_memory[1], 0, VK_WHOLE_SIZE, 0, &data2);

  data3 = NULL;
  vkMapMemory(gpu_api->vulkan_state->device, grass->grass_shader.grass_compute_memory[2], 0, VK_WHOLE_SIZE, 0, &data3);

  d_a = data1;
  d_b = data2;
  d_c = data3;

  vkUnmapMemory(gpu_api->vulkan_state->device, grass->grass_shader.grass_compute_memory[0]);
  vkUnmapMemory(gpu_api->vulkan_state->device, grass->grass_shader.grass_compute_memory[1]);
  vkUnmapMemory(gpu_api->vulkan_state->device, grass->grass_shader.grass_compute_memory[2]);

  // Other render stuff
  graphics_utils_update_vertex_buffer(gpu_api->vulkan_state, grass->mesh->vertices, &grass->vertex_buffer, &grass->vertex_buffer_memory);
  graphics_utils_update_index_buffer(gpu_api->vulkan_state, grass->mesh->indices, &grass->index_buffer, &grass->index_buffer_memory);

  vkCmdBindPipeline(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, grass->grass_shader.grass_render_shader.graphics_pipeline);

  VkBuffer vertex_buffers[] = {grass->vertex_buffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, 0, 1, vertex_buffers, offsets);
  vkCmdBindIndexBuffer(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, grass->index_buffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdBindDescriptorSets(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, grass->grass_shader.grass_render_shader.pipeline_layout, 0, 1, &grass->descriptor_set, 0, NULL);
  vkCmdDrawIndexed(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, grass->mesh->indices->size, 1, 0, 0, 0);
}

void grass_update_uniforms(struct Grass* grass, struct GPUAPI* gpu_api) {
  struct GrassUniformBufferObject ubos = {{{0}}};
  ubos.proj = gpu_api->vulkan_state->gbuffer->projection_matrix;
  ubos.proj.vecs[1].data[1] *= -1;

  ubos.view = gpu_api->vulkan_state->gbuffer->view_matrix;

  ubos.model = MAT4_IDENTITY;

  void* data;
  vkMapMemory(gpu_api->vulkan_state->device, grass->uniform_buffers_memory, 0, sizeof(struct GrassUniformBufferObject), 0, &data);
  memcpy(data, &ubos, sizeof(struct GrassUniformBufferObject));
  vkUnmapMemory(gpu_api->vulkan_state->device, grass->uniform_buffers_memory);
}

//void grass_recreate(struct Grass* grass, struct GPUAPI* gpu_api) {
//  grass_shader_delete(&grass->grass_shader, gpu_api);
//  grass_shader_init(&grass->grass_shader, gpu_api);
//
//  grass_vulkan_cleanup(grass, gpu_api);
//
//  graphics_utils_setup_vertex_buffer(gpu_api->vulkan_state, grass->mesh->vertices, &grass->vertex_buffer, &grass->vertex_buffer_memory);
//  graphics_utils_setup_index_buffer(gpu_api->vulkan_state, grass->mesh->indices, &grass->index_buffer, &grass->index_buffer_memory);
//  graphics_utils_setup_uniform_buffer(gpu_api->vulkan_state, sizeof(struct GrassUniformBufferObject), &grass->uniform_buffer, &grass->uniform_buffers_memory);
//  graphics_utils_setup_descriptor(gpu_api->vulkan_state, grass->grass_shader.grass_render_shader.descriptor_set_layout, grass->grass_shader.grass_render_shader.descriptor_pool, &grass->descriptor_set);
//
//  VkWriteDescriptorSet dcs[1] = {0};
//  graphics_utils_setup_descriptor_buffer(gpu_api->vulkan_state, dcs, 0, &grass->descriptor_set, (VkDescriptorBufferInfo[]){graphics_utils_setup_descriptor_buffer_info(sizeof(struct GrassUniformBufferObject), &grass->uniform_buffer)});
//  vkUpdateDescriptorSets(gpu_api->vulkan_state->device, 1, dcs, 0, NULL);
//}
