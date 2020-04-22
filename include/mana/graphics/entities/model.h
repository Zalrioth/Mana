#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "mana/core/memoryallocator.h"
//
#include "mana/core/xmlparser.h"
#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/render/vulkanrenderer.h"
#include "mana/graphics/shaders/shader.h"
#include "mana/graphics/utilities/collada/modelgeometry.h"
#include "mana/graphics/utilities/collada/modelskeleton.h"
#include "mana/graphics/utilities/collada/modelskinning.h"
#include "mana/graphics/utilities/mesh.h"

#define MAX_JOINTS 50

struct ModelUniformBufferObject {
  alignas(16) mat4 model;
  alignas(16) mat4 view;
  alignas(16) mat4 proj;
  alignas(16) mat4 jointTransforms[MAX_JOINTS];
  alignas(16) vec3 lightDirection;
};

struct Model {
  struct SkeletonData* joints;
  struct Mesh* model_mesh;
  struct Texture* model_texture;

  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_buffer_memory;
  VkBuffer index_buffer;
  VkDeviceMemory index_buffer_memory;
  VkBuffer uniform_buffer;
  VkDeviceMemory uniform_buffers_memory;
  VkDescriptorSet descriptor_set;
};

enum {
  MODEL_SUCCESS = 1
};

int model_init(struct Model* model, struct VulkanRenderer* vulkan_renderer, char* mode_path, char* texture_path, int max_weights, struct Shader* shader);

#endif  // MODEL_H
