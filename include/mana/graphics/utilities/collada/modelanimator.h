#pragma once
#ifndef MODEL_ANIMATOR
#define MODEL_ANIMATOR

#include "mana/core/memoryallocator.h"
//
#include <cstorage/cstorage.h>
#include <ubermath/ubermath.h>

#include "mana/core/corecommon.h"
#include "mana/graphics/entities/model.h"
#include "mana/graphics/utilities/mesh.h"
#include "xmlnode.h"

struct ModelJoint;

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
  quat rotation;
};

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
void animator_apply_pose_to_joints(struct Map* current_pose, struct ModelJoint* joint, mat4 parent_transform);
void animator_get_previous_and_next_frames(struct Animator* animator, struct KeyFrame** previous_frame, struct KeyFrame** next_frame);
float animator_calculate_progression(struct Animator* animator, struct KeyFrame* previous_frame, struct KeyFrame* next_frame);
struct Map* animator_interpolate_poses(struct KeyFrame* previous_frame, struct KeyFrame* next_frame, float progression);

#endif  // MODEL_ANIMATOR
