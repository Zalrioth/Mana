#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "mana/core/memoryallocator.h"
//
#include "mana/core/xmlparser.h"
#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/render/vulkanrenderer.h"
#include "mana/graphics/shaders/shader.h"
#include "mana/graphics/utilities/collada/modelanimation.h"
#include "mana/graphics/utilities/collada/modelanimator.h"
#include "mana/graphics/utilities/collada/modelgeometry.h"
#include "mana/graphics/utilities/collada/modelskeleton.h"
#include "mana/graphics/utilities/collada/modelskinning.h"
#include "mana/graphics/utilities/mesh.h"

#define MAX_JOINTS 50

struct Shader;
struct KeyFrame;
struct KeyFrameData;
struct JointTransform;
struct JointTransformData;

struct ModelUniformBufferObject {
  alignas(16) mat4 model;
  alignas(16) mat4 view;
  alignas(16) mat4 proj;
  alignas(16) mat4 joint_transforms[MAX_JOINTS];
  alignas(16) vec3 light_direction;
};

struct ModelCache {
  struct SkeletonData* joints;
  struct Mesh* model_mesh;
  // Pointer to texture in texture cache
  struct Texture* model_texture;
};

struct Joint {
  int index;
  char* name;
  struct ArrayList* children;
  mat4 animation_transform;
  mat4 local_bind_transform;
  mat4 inverse_bind_transform;
};

static inline void joint_init(struct Joint* joint, int index, char* name, mat4 bind_local_transform) {
  glm_mat4_identity(joint->animation_transform);
  glm_mat4_identity(joint->inverse_bind_transform);
  joint->children = malloc(sizeof(struct ArrayList));
  array_list_init(joint->children);
  joint->index = index;
  joint->name = name;
  glm_mat4_copy(bind_local_transform, joint->local_bind_transform);
}

static inline void joint_calc_inverse_bind_transform(struct Joint* joint, mat4 parent_bind_transform) {
  mat4 bind_transform = GLM_MAT4_IDENTITY_INIT;
  glm_mat4_mul(parent_bind_transform, joint->local_bind_transform, bind_transform);
  glm_mat4_inv(bind_transform, joint->inverse_bind_transform);
  for (int child_num = 0; child_num < array_list_size(joint->children); child_num++) {
    struct Joint* child_joint = (struct Joint*)array_list_get(joint->children, child_num);
    joint_calc_inverse_bind_transform(child_joint, bind_transform);
  }
}

struct Model {
  struct ModelCache* model_raw;
  struct Joint* root_joint;
  int joint_count;
  struct Animator* animator;
  struct Animation* animation;

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

int model_init(struct Model* model, struct VulkanRenderer* vulkan_renderer, char* node_path, char* texture_path, int max_weights, struct Shader* shader);
struct Joint* model_create_joints(struct JointData* root_joint_data);
struct KeyFrame* model_create_key_frame(struct KeyFrameData* data);
struct JointTransform* model_create_transform(struct JointTransformData* data);
void model_get_joint_transforms(struct Joint* head_joint, mat4 dest[MAX_JOINTS]);

#endif  // MODEL_H
