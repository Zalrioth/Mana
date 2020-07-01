#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "mana/core/memoryallocator.h"
//
#include <mana/core/gpuapi.h>

#include "mana/core/vulkancore.h"
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

struct GPUAPI;
struct Shader;
struct KeyFrame;
struct KeyFrameData;
struct JointTransform;
struct JointTransformData;

enum FilterType {
  FILTER_NEAREST = 0,
  FILTER_LINEAR
};

// TODO: Move to math library
static inline void mat4_to_collada_quaternion(mat4 matrix, versor dest) {
  float diagonal = matrix[0][0] + matrix[1][1] + matrix[2][2];
  if (diagonal > 0) {
    float w4 = (float)(sqrtf(diagonal + 1.0f) * 2.0f);
    dest[3] = w4 / 4.0f;
    dest[0] = (matrix[2][1] - matrix[1][2]) / w4;
    dest[1] = (matrix[0][2] - matrix[2][0]) / w4;
    dest[2] = (matrix[1][0] - matrix[0][1]) / w4;
  } else if ((matrix[0][0] > matrix[1][1]) && (matrix[0][0] > matrix[2][2])) {
    float x4 = (float)(sqrtf(1.0f + matrix[0][0] - matrix[1][1] - matrix[2][2]) * 2.0f);
    dest[3] = (matrix[2][1] - matrix[1][2]) / x4;
    dest[0] = x4 / 4.0f;
    dest[1] = (matrix[0][1] + matrix[1][0]) / x4;
    dest[2] = (matrix[0][2] + matrix[2][0]) / x4;
  } else if (matrix[1][1] > matrix[2][2]) {
    float y4 = (float)(sqrtf(1.0f + matrix[1][1] - matrix[0][0] - matrix[2][2]) * 2.0f);
    dest[3] = (matrix[0][2] - matrix[2][0]) / y4;
    dest[0] = (matrix[0][1] + matrix[1][0]) / y4;
    dest[1] = y4 / 4.0f;
    dest[2] = (matrix[1][2] + matrix[2][1]) / y4;
  } else {
    float z4 = (float)(sqrtf(1.0f + matrix[2][2] - matrix[0][0] - matrix[1][1]) * 2.0f);
    dest[3] = (matrix[1][0] - matrix[0][1]) / z4;
    dest[0] = (matrix[0][2] + matrix[2][0]) / z4;
    dest[1] = (matrix[1][2] + matrix[2][1]) / z4;
    dest[2] = z4 / 4.0f;
  }

  float mag = sqrtf(dest[3] * dest[3] + dest[0] * dest[0] + dest[1] * dest[1] + dest[2] * dest[2]);
  dest[3] /= mag;
  dest[0] /= mag;
  dest[1] /= mag;
  dest[2] /= mag;
}

struct ModelUniformBufferObject {
  alignas(16) mat4 model;
  alignas(16) mat4 view;
  alignas(16) mat4 proj;
  alignas(16) vec3 camera_pos;
  alignas(16) mat4 joint_transforms[MAX_JOINTS];
};

struct ModelStaticUniformBufferObject {
  alignas(16) mat4 model;
  alignas(16) mat4 view;
  alignas(16) mat4 proj;
  alignas(16) vec3 camera_pos;
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
  joint->name = strdup(name);
  glm_mat4_copy(bind_local_transform, joint->local_bind_transform);
}

static inline void joint_calc_inverse_bind_transform(struct Joint* joint, mat4 parent_bind_transform) {
  mat4 bind_transform = GLM_MAT4_ZERO_INIT;
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
  VkBuffer lighting_uniform_buffer;
  VkDeviceMemory lighting_uniform_buffers_memory;
  VkDescriptorSet descriptor_set;
};

enum {
  MODEL_SUCCESS = 1
};

int model_init(struct Model* model, struct GPUAPI* gpu_api, char* node_path, char* texture_path, int max_weights, struct Shader* shader, enum FilterType filter_type);
void model_delete(struct GPUAPI* gpu_api, struct Model* model);
struct Joint* model_create_joints(struct JointData* root_joint_data);
struct KeyFrame* model_create_key_frame(struct KeyFrameData* data);
struct JointTransform* model_create_transform(struct JointTransformData* data);
void model_get_joint_transforms(struct Joint* head_joint, mat4 dest[MAX_JOINTS]);

#endif  // MODEL_H
