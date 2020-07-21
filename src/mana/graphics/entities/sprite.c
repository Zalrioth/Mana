#include "mana/graphics/entities/sprite.h"

//void calc_normal(vec3 p1, vec3 p2, vec3 p3, float* dest) {
//  vec3 v1;
//  glm_vec3_sub(p2, p1, v1);
//
//  vec3 v2;
//  glm_vec3_sub(p3, p1, v2);
//
//  glm_vec3_crossn(v1, v2, dest);
//}

int sprite_init(struct Sprite* sprite, struct GPUAPI* gpu_api, struct Shader* shader, struct Texture* texture) {
  sprite->image_mesh = calloc(1, sizeof(struct Mesh));
  mesh_sprite_init(sprite->image_mesh);

  sprite->image_texture = texture;
  sprite->shader = shader;

  vec3 pos1 = (vec3){.x = -0.5f, .y = -0.5f, .z = 0.0f};
  vec3 pos2 = (vec3){.x = 0.5f, .y = -0.5f, .z = 0.0f};
  vec3 pos3 = (vec3){.x = 0.5f, .y = 0.5f, .z = 0.0f};
  vec3 pos4 = (vec3){.x = -0.5f, .y = 0.5f, .z = 0.0f};

  vec2 uv1 = (vec2){.u = 1.0f, .v = 0.0f};
  vec2 uv2 = (vec2){.u = 0.0f, .v = 0.0f};
  vec2 uv3 = (vec2){.u = 0.0f, .v = 1.0f};
  vec2 uv4 = (vec2){.u = 1.0f, .v = 1.0f};

  mesh_sprite_assign_vertex(sprite->image_mesh->vertices, pos1.x, pos1.y, pos1.z, uv1.u, uv1.v);
  mesh_sprite_assign_vertex(sprite->image_mesh->vertices, pos2.x, pos2.y, pos2.z, uv2.u, uv2.v);
  mesh_sprite_assign_vertex(sprite->image_mesh->vertices, pos3.x, pos3.y, pos3.z, uv3.u, uv3.v);
  mesh_sprite_assign_vertex(sprite->image_mesh->vertices, pos4.x, pos4.y, pos4.z, uv4.u, uv4.v);

  mesh_assign_indice(sprite->image_mesh->indices, 0);
  mesh_assign_indice(sprite->image_mesh->indices, 1);
  mesh_assign_indice(sprite->image_mesh->indices, 2);
  mesh_assign_indice(sprite->image_mesh->indices, 2);
  mesh_assign_indice(sprite->image_mesh->indices, 3);
  mesh_assign_indice(sprite->image_mesh->indices, 0);

  graphics_utils_setup_vertex_buffer(gpu_api->vulkan_state, sprite->image_mesh->vertices, &sprite->vertex_buffer, &sprite->vertex_buffer_memory);
  graphics_utils_setup_index_buffer(gpu_api->vulkan_state, sprite->image_mesh->indices, &sprite->index_buffer, &sprite->index_buffer_memory);
  graphics_utils_setup_uniform_buffer(gpu_api->vulkan_state, sizeof(struct SpriteUniformBufferObject), &sprite->uniform_buffer, &sprite->uniform_buffers_memory);
  graphics_utils_setup_descriptor(gpu_api->vulkan_state, shader->descriptor_set_layout, shader->descriptor_pool, &sprite->descriptor_set);

  VkWriteDescriptorSet dcs[2] = {0};

  graphics_utils_setup_descriptor_buffer(gpu_api->vulkan_state, dcs, 0, &sprite->descriptor_set, (VkDescriptorBufferInfo[]){graphics_utils_setup_descriptor_buffer_info(sizeof(struct SpriteUniformBufferObject), &sprite->uniform_buffer)});
  graphics_utils_setup_descriptor_image(gpu_api->vulkan_state, dcs, 1, &sprite->descriptor_set, (VkDescriptorImageInfo[]){graphics_utils_setup_descriptor_image_info(&sprite->image_texture->texture_image_view, &sprite->image_texture->texture_sampler)});

  vkUpdateDescriptorSets(gpu_api->vulkan_state->device, 2, dcs, 0, NULL);

  return SPRITE_SUCCESS;
}

static inline void sprite_vulkan_cleanup(struct Sprite* sprite, struct GPUAPI* gpu_api) {
  vkDestroyBuffer(gpu_api->vulkan_state->device, sprite->index_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, sprite->index_buffer_memory, NULL);

  vkDestroyBuffer(gpu_api->vulkan_state->device, sprite->vertex_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, sprite->vertex_buffer_memory, NULL);

  vkDestroyBuffer(gpu_api->vulkan_state->device, sprite->uniform_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, sprite->uniform_buffers_memory, NULL);
}

void sprite_delete(struct Sprite* sprite, struct GPUAPI* gpu_api) {
  sprite_vulkan_cleanup(sprite, gpu_api);
  mesh_delete(sprite->image_mesh);
  free(sprite->image_mesh);
}

void sprite_render(struct Sprite* sprite, struct GPUAPI* gpu_api, float delta_time) {
  vkCmdBindPipeline(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, sprite->shader->graphics_pipeline);

  VkBuffer vertex_buffers[] = {sprite->vertex_buffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, 0, 1, vertex_buffers, offsets);
  vkCmdBindIndexBuffer(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, sprite->index_buffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdBindDescriptorSets(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, sprite->shader->pipeline_layout, 0, 1, &sprite->descriptor_set, 0, NULL);
  vkCmdDrawIndexed(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, sprite->image_mesh->indices->size, 1, 0, 0, 0);
}

void sprite_recreate(struct Sprite* sprite, struct GPUAPI* gpu_api) {
  sprite_vulkan_cleanup(sprite, gpu_api);

  graphics_utils_setup_vertex_buffer(gpu_api->vulkan_state, sprite->image_mesh->vertices, &sprite->vertex_buffer, &sprite->vertex_buffer_memory);
  graphics_utils_setup_index_buffer(gpu_api->vulkan_state, sprite->image_mesh->indices, &sprite->index_buffer, &sprite->index_buffer_memory);
  graphics_utils_setup_uniform_buffer(gpu_api->vulkan_state, sizeof(struct SpriteUniformBufferObject), &sprite->uniform_buffer, &sprite->uniform_buffers_memory);
  graphics_utils_setup_descriptor(gpu_api->vulkan_state, sprite->shader->descriptor_set_layout, sprite->shader->descriptor_pool, &sprite->descriptor_set);

  VkWriteDescriptorSet dcs[2] = {0};

  graphics_utils_setup_descriptor_buffer(gpu_api->vulkan_state, dcs, 0, &sprite->descriptor_set, (VkDescriptorBufferInfo[]){graphics_utils_setup_descriptor_buffer_info(sizeof(struct SpriteUniformBufferObject), &sprite->uniform_buffer)});
  graphics_utils_setup_descriptor_image(gpu_api->vulkan_state, dcs, 1, &sprite->descriptor_set, (VkDescriptorImageInfo[]){graphics_utils_setup_descriptor_image_info(&sprite->image_texture->texture_image_view, &sprite->image_texture->texture_sampler)});

  vkUpdateDescriptorSets(gpu_api->vulkan_state->device, 2, dcs, 0, NULL);
  vkUpdateDescriptorSets(gpu_api->vulkan_state->device, 2, dcs, 0, NULL);
}
