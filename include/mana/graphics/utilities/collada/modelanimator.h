#pragma once
#ifndef MODEL_ANIMATOR
#define MODEL_ANIMATOR

#include "mana/core/memoryallocator.h"
//
#include <cglm/cglm.h>
#include <cstorage/cstorage.h>

#include "mana/core/corecommon.h"
#include "mana/core/xmlnode.h"
#include "mana/graphics/entities/model.h"
#include "mana/graphics/utilities/mesh.h"

struct Joint;

struct KeyFrame {
  float time_step;
  struct Map* pose;
};

static inline void key_frame_init(struct KeyFrame* key_frame, float time_stamp, struct Map* joint_key_frames) {
  key_frame->time_step = time_stamp;
  key_frame->pose = joint_key_frames;
}

struct Animation {
  float length;
  struct ArrayList* key_frames;
};

static inline void animation_init(struct Animation* animation, float length_in_seconds, struct ArrayList* frames) {
  animation->key_frames = frames;
  animation->length = length_in_seconds;
}

struct JointTransform {
  vec3 position;
  versor rotation;
};

static inline void joint_transform_init(struct JointTransform* joint_transform, vec3 position, versor rotation) {
  glm_vec3_copy(position, joint_transform->position);
  glm_quat_copy(rotation, joint_transform->rotation);
}

static inline void joint_transform_get_local_transform(struct JointTransform* joint_transform, mat4 dest) {
  glm_translate(dest, joint_transform->position);
  mat4 rotation_matrix = GLM_MAT4_ZERO_INIT;
  glm_quat_mat4(joint_transform->rotation, rotation_matrix);
  glm_mat4_mul(dest, rotation_matrix, dest);
}

static inline void model_interpolate_quat(versor a, versor b, float blend, versor dest) {
  //versor result = {0.0f, 0.0f, 0.0f, 1.0f};
  float dot = a[3] * b[3] + a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
  float blendI = 1.0f - blend;
  if (dot < 0) {
    dest[3] = blendI * a[3] + blend * -b[3];
    dest[0] = blendI * a[0] + blend * -b[0];
    dest[1] = blendI * a[1] + blend * -b[1];
    dest[2] = blendI * a[2] + blend * -b[2];
  } else {
    dest[3] = blendI * a[3] + blend * b[3];
    dest[0] = blendI * a[0] + blend * b[0];
    dest[1] = blendI * a[1] + blend * b[1];
    dest[2] = blendI * a[2] + blend * b[2];
  }
  glm_quat_norm(dest);
  //result.normalize();
  //return result;
}

static inline void model_interpolate_vec3(vec3 start, vec3 end, float progression, vec3 dest) {
  dest[0] = start[0] + (end[0] - start[0]) * progression;
  dest[1] = start[1] + (end[1] - start[1]) * progression;
  dest[2] = start[2] + (end[2] - start[2]) * progression;
}

static inline struct JointTransform* joint_transform_interpolate(struct JointTransform* frame_a, struct JointTransform* frame_b, float progression) {
  vec3 pos = GLM_VEC3_ZERO_INIT;
  //glm_vec3_lerp(frame_a->position, frame_b->position, progression, pos);
  model_interpolate_vec3(frame_a->position, frame_b->position, progression, pos);
  versor rot = GLM_QUAT_IDENTITY_INIT;
  model_interpolate_quat(frame_a->rotation, frame_b->rotation, progression, rot);
  //glm_quat_lerp(frame_a->rotation, frame_b->rotation, progression, rot);
  struct JointTransform* joint_transform = malloc(sizeof(struct JointTransform));
  joint_transform_init(joint_transform, pos, rot);
  return joint_transform;
}

struct Animator {
  struct Model* entity;
  struct Animation* current_animation;
  float animation_time;
};

void animator_init(struct Animator* animator, struct Model* entity);
void animator_do_animation(struct Animator* animator, struct Animation* animation);
void animator_update(struct Animator* animator, float delta_time);
void animator_increase_animation_time(struct Animator* animator, float delta_time);
struct Map* animator_calculate_current_animation_pose(struct Animator* animator);
void animator_apply_pose_to_joints(struct Map* current_pose, struct Joint* joint, mat4 parent_transform);
void animator_get_previous_and_next_frames(struct Animator* animator, struct KeyFrame** previous_frame, struct KeyFrame** next_frame);
float animator_calculate_progression(struct Animator* animator, struct KeyFrame* previous_frame, struct KeyFrame* next_frame);
struct Map* animator_interpolate_poses(struct KeyFrame* previous_frame, struct KeyFrame* next_frame, float progression);

#endif  // MODEL_ANIMATOR
