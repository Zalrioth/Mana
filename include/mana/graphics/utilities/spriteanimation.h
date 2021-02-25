#pragma once
#ifndef SPRITE_ANIMATION_H
#define SPRITE_ANIMATION_H

#include "mana/core/memoryallocator.h"
//
#include <mana/core/gpuapi.h>

#include "mana/core/vulkancore.h"
#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/shaders/shader.h"
#include "mana/graphics/utilities/texture.h"

struct SpriteAnimationUniformBufferObject {
  alignas(16) mat4 model;
  alignas(16) mat4 view;
  alignas(16) mat4 proj;
  alignas(16) vec3 frame_pos;
};

struct SpriteAnimation {
  struct Mesh* image_mesh;
  struct Texture* image_texture;

  vec3 position;
  vec3 scale;
  quat rotation;
  float width;
  float height;

  vec3 frame_pos;
  int total_frames;
  int current_frame;
  float total_animation_length;
  float frame_length;
  float current_animation_time;
  int padding;
  float direction;

  struct Shader* shader;
  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_buffer_memory;
  VkBuffer index_buffer;
  VkDeviceMemory index_buffer_memory;
  VkBuffer uniform_buffer;
  VkDeviceMemory uniform_buffers_memory;
  VkDescriptorSet descriptor_set;
};

enum {
  SPRITE_ANIMATION_SUCCESS = 1
};

int sprite_animation_init(struct SpriteAnimation* sprite_animation, struct GPUAPI* gpu_api, struct Shader* shader, struct Texture* texture, int frames, float frame_length, int padding);
void sprite_animation_delete(struct SpriteAnimation* sprite_animation, struct GPUAPI* gpu_api);
void sprite_animation_render(struct SpriteAnimation* sprite_animation, struct GPUAPI* gpu_api, float delta_time);
void sprite_animation_update_uniforms(struct SpriteAnimation* sprite_animation, struct GPUAPI* gpu_api);
void sprite_animation_recreate(struct SpriteAnimation* sprite_animation, struct GPUAPI* gpu_api);
void sprite_animation_set_frame(struct SpriteAnimation* sprite_animation, int frame);
void sprite_animation_update(struct SpriteAnimation* sprite_animation, float delta_time);

#endif  // SPRITE_ANIMATION_H
