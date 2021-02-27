#pragma once
#ifndef SPRITE_H
#define SPRITE_H

#include "mana/core/memoryallocator.h"
//
#include <mana/core/gpuapi.h>

#include "mana/core/vulkancore.h"
#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/shaders/shader.h"
#include "mana/graphics/utilities/texture.h"

struct SpriteUniformBufferObject {
  alignas(16) mat4 model;
  alignas(16) mat4 view;
  alignas(16) mat4 proj;
};

struct Sprite {
  struct Mesh* image_mesh;
  struct Texture* image_texture;

  vec3 position;
  vec3 scale;
  quat rotation;
  float width;
  float height;

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
  SPRITE_SUCCESS = 1
};

int sprite_init(struct Sprite* sprite, struct GPUAPI* gpu_api, struct Shader* shader, struct Texture* texture);
void sprite_delete(struct Sprite* sprite, struct GPUAPI* gpu_api);
void sprite_render(struct Sprite* sprite, struct GPUAPI* gpu_api);
void sprite_update_uniforms(struct Sprite* sprite, struct GPUAPI* gpu_api);
void sprite_recreate(struct Sprite* sprite, struct GPUAPI* gpu_api);

#endif  // SPRITE_H
