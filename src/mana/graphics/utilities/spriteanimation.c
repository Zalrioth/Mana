#include "mana/graphics/utilities/spriteanimation.h"

// TODO: Move this to entities?
int sprite_animation_init(struct SpriteAnimation* sprite_animation, struct GPUAPI* gpu_api, struct Shader* shader, struct Texture* texture, int frames, float frame_length, int padding) {
  sprite_animation->image_mesh = calloc(1, sizeof(struct Mesh));
  mesh_sprite_init(sprite_animation->image_mesh);

  sprite_animation->image_texture = texture;
  sprite_animation->shader = shader;
  sprite_animation->scale = VEC3_ONE;
  sprite_animation->rotation = QUAT_DEFAULT;
  sprite_animation->frame_pos = VEC3_ZERO;
  sprite_animation->direction = 1.0f;
  sprite_animation->animate = 1;
  sprite_animation->loop = 1;

  sprite_animation->total_frames = frames;
  sprite_animation->frame_length = frame_length;
  sprite_animation->padding = padding;

  sprite_animation->total_animation_length = frame_length * frames;
  sprite_animation->current_animation_time = 0.0f;

  float tex_norm_width = (texture->width / frames) / 100.0f;
  float tex_norm_height = texture->height / 100.0f;

  float tex_norm_width_half = tex_norm_width / 2.0f;
  float tex_norm_height_half = tex_norm_height / 2.0f;

  //float pixel_width = 1.0f - (((float)texture->width - 1) / (float)texture->width);
  //float pixel_height = 1.0f - (((float)texture->height - 1) / (float)texture->height);

  //sprite_animation->width = tex_norm_width - pixel_width;
  //sprite_animation->height = tex_norm_height - pixel_height;

  sprite_animation->width = tex_norm_width;
  sprite_animation->height = tex_norm_height;

  vec3 pos1 = (vec3){.x = -tex_norm_width_half, .y = -tex_norm_height_half, .z = 0.0f};
  vec3 pos2 = (vec3){.x = tex_norm_width_half, .y = -tex_norm_height_half, .z = 0.0f};
  vec3 pos3 = (vec3){.x = tex_norm_width_half, .y = tex_norm_height_half, .z = 0.0f};
  vec3 pos4 = (vec3){.x = -tex_norm_width_half, .y = tex_norm_height_half, .z = 0.0f};

  //vec2 uv1 = (vec2){.u = 1.0f - pixel_width, .v = 1.0f - pixel_height};
  //vec2 uv2 = (vec2){.u = 0.0f + pixel_width, .v = 1.0f - pixel_height};
  //vec2 uv3 = (vec2){.u = 0.0f + pixel_width, .v = 0.0f + pixel_height};
  //vec2 uv4 = (vec2){.u = 1.0f - pixel_width, .v = 0.0f + pixel_height};

  vec2 uv1 = (vec2){.u = 1.0f / frames, .v = 1.0f};
  vec2 uv2 = (vec2){.u = 0.0f, .v = 1.0f};
  vec2 uv3 = (vec2){.u = 0.0f, .v = 0.0f};
  vec2 uv4 = (vec2){.u = 1.0f / frames, .v = 0.0f};

  mesh_sprite_assign_vertex(sprite_animation->image_mesh->vertices, pos1.x, pos1.y, pos1.z, uv1.u, uv1.v);
  mesh_sprite_assign_vertex(sprite_animation->image_mesh->vertices, pos2.x, pos2.y, pos2.z, uv2.u, uv2.v);
  mesh_sprite_assign_vertex(sprite_animation->image_mesh->vertices, pos3.x, pos3.y, pos3.z, uv3.u, uv3.v);
  mesh_sprite_assign_vertex(sprite_animation->image_mesh->vertices, pos4.x, pos4.y, pos4.z, uv4.u, uv4.v);

  mesh_assign_indice(sprite_animation->image_mesh->indices, 0);
  mesh_assign_indice(sprite_animation->image_mesh->indices, 1);
  mesh_assign_indice(sprite_animation->image_mesh->indices, 2);
  mesh_assign_indice(sprite_animation->image_mesh->indices, 2);
  mesh_assign_indice(sprite_animation->image_mesh->indices, 3);
  mesh_assign_indice(sprite_animation->image_mesh->indices, 0);

  graphics_utils_setup_vertex_buffer(gpu_api->vulkan_state, sprite_animation->image_mesh->vertices, &sprite_animation->vertex_buffer, &sprite_animation->vertex_buffer_memory);
  graphics_utils_setup_index_buffer(gpu_api->vulkan_state, sprite_animation->image_mesh->indices, &sprite_animation->index_buffer, &sprite_animation->index_buffer_memory);
  graphics_utils_setup_uniform_buffer(gpu_api->vulkan_state, sizeof(struct SpriteAnimationUniformBufferObject), &sprite_animation->uniform_buffer, &sprite_animation->uniform_buffers_memory);
  graphics_utils_setup_descriptor(gpu_api->vulkan_state, shader->descriptor_set_layout, shader->descriptor_pool, &sprite_animation->descriptor_set);

  VkWriteDescriptorSet dcs[2] = {0};
  graphics_utils_setup_descriptor_buffer(gpu_api->vulkan_state, dcs, 0, &sprite_animation->descriptor_set, (VkDescriptorBufferInfo[]){graphics_utils_setup_descriptor_buffer_info(sizeof(struct SpriteAnimationUniformBufferObject), &sprite_animation->uniform_buffer)});
  graphics_utils_setup_descriptor_image(gpu_api->vulkan_state, dcs, 1, &sprite_animation->descriptor_set, (VkDescriptorImageInfo[]){graphics_utils_setup_descriptor_image_info(&sprite_animation->image_texture->texture_image_view, &sprite_animation->image_texture->texture_sampler)});
  vkUpdateDescriptorSets(gpu_api->vulkan_state->device, 2, dcs, 0, NULL);

  return SPRITE_ANIMATION_SUCCESS;
}

static inline void sprite_animation_vulkan_cleanup(struct SpriteAnimation* sprite_animation, struct GPUAPI* gpu_api) {
  vkDestroyBuffer(gpu_api->vulkan_state->device, sprite_animation->index_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, sprite_animation->index_buffer_memory, NULL);

  vkDestroyBuffer(gpu_api->vulkan_state->device, sprite_animation->vertex_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, sprite_animation->vertex_buffer_memory, NULL);

  vkDestroyBuffer(gpu_api->vulkan_state->device, sprite_animation->uniform_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, sprite_animation->uniform_buffers_memory, NULL);
}

void sprite_animation_delete(struct SpriteAnimation* sprite_animation, struct GPUAPI* gpu_api) {
  sprite_animation_vulkan_cleanup(sprite_animation, gpu_api);
  mesh_delete(sprite_animation->image_mesh);
  free(sprite_animation->image_mesh);
}

void sprite_animation_render(struct SpriteAnimation* sprite_animation, struct GPUAPI* gpu_api) {
  vkCmdBindPipeline(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, sprite_animation->shader->graphics_pipeline);

  VkBuffer vertex_buffers[] = {sprite_animation->vertex_buffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, 0, 1, vertex_buffers, offsets);
  vkCmdBindIndexBuffer(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, sprite_animation->index_buffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdBindDescriptorSets(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, sprite_animation->shader->pipeline_layout, 0, 1, &sprite_animation->descriptor_set, 0, NULL);
  vkCmdDrawIndexed(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, sprite_animation->image_mesh->indices->size, 1, 0, 0, 0);
}

void sprite_animation_update_uniforms(struct SpriteAnimation* sprite_animation, struct GPUAPI* gpu_api) {
  struct SpriteAnimationUniformBufferObject ubos = {{{0}}};
  ubos.proj = gpu_api->vulkan_state->gbuffer->projection_matrix;
  ubos.proj.vecs[1].data[1] *= -1;

  ubos.view = gpu_api->vulkan_state->gbuffer->view_matrix;

  ubos.model = mat4_translate(MAT4_IDENTITY, sprite_animation->position);
  ubos.model = mat4_mul(ubos.model, quaternion_to_mat4(quaternion_normalise(sprite_animation->rotation)));
  ubos.model = mat4_scale(ubos.model, sprite_animation->scale);

  ubos.frame_pos = sprite_animation->frame_pos;

  void* data;
  vkMapMemory(gpu_api->vulkan_state->device, sprite_animation->uniform_buffers_memory, 0, sizeof(struct SpriteAnimationUniformBufferObject), 0, &data);
  memcpy(data, &ubos, sizeof(struct SpriteAnimationUniformBufferObject));
  vkUnmapMemory(gpu_api->vulkan_state->device, sprite_animation->uniform_buffers_memory);
}

void sprite_animation_recreate(struct SpriteAnimation* sprite_animation, struct GPUAPI* gpu_api) {
  sprite_animation_vulkan_cleanup(sprite_animation, gpu_api);

  graphics_utils_setup_vertex_buffer(gpu_api->vulkan_state, sprite_animation->image_mesh->vertices, &sprite_animation->vertex_buffer, &sprite_animation->vertex_buffer_memory);
  graphics_utils_setup_index_buffer(gpu_api->vulkan_state, sprite_animation->image_mesh->indices, &sprite_animation->index_buffer, &sprite_animation->index_buffer_memory);
  graphics_utils_setup_uniform_buffer(gpu_api->vulkan_state, sizeof(struct SpriteAnimationUniformBufferObject), &sprite_animation->uniform_buffer, &sprite_animation->uniform_buffers_memory);
  graphics_utils_setup_descriptor(gpu_api->vulkan_state, sprite_animation->shader->descriptor_set_layout, sprite_animation->shader->descriptor_pool, &sprite_animation->descriptor_set);

  VkWriteDescriptorSet dcs[2] = {0};

  graphics_utils_setup_descriptor_buffer(gpu_api->vulkan_state, dcs, 0, &sprite_animation->descriptor_set, (VkDescriptorBufferInfo[]){graphics_utils_setup_descriptor_buffer_info(sizeof(struct SpriteAnimationUniformBufferObject), &sprite_animation->uniform_buffer)});
  graphics_utils_setup_descriptor_image(gpu_api->vulkan_state, dcs, 1, &sprite_animation->descriptor_set, (VkDescriptorImageInfo[]){graphics_utils_setup_descriptor_image_info(&sprite_animation->image_texture->texture_image_view, &sprite_animation->image_texture->texture_sampler)});

  vkUpdateDescriptorSets(gpu_api->vulkan_state->device, 2, dcs, 0, NULL);
  vkUpdateDescriptorSets(gpu_api->vulkan_state->device, 2, dcs, 0, NULL);
}

void sprite_animation_update(struct SpriteAnimation* sprite_animation, float delta_time) {
  if (sprite_animation->animate == 1) {
    sprite_animation->current_animation_time += delta_time;
    if (sprite_animation->current_animation_time > sprite_animation->total_animation_length)
      if (sprite_animation->loop == 1)
        sprite_animation->current_animation_time = 0.0f;
      else
        sprite_animation->current_animation_time = 0.999f;
  }

  sprite_animation->current_frame = (int)((sprite_animation->current_animation_time / sprite_animation->total_animation_length) * sprite_animation->total_frames);
  float offset = 0.0f;
  if (sprite_animation->direction > 0.0f)
    offset = 1.0f / sprite_animation->total_frames;
  sprite_animation->frame_pos = (vec3){.x = ((float)sprite_animation->current_frame / sprite_animation->total_frames) + offset, .y = 0.0f, .z = sprite_animation->direction};
}
