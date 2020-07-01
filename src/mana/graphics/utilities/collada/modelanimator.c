#include "mana/graphics/utilities/collada/modelanimator.h"

void animator_init(struct Animator* animator, struct Model* entity) {
  animator->entity = entity;
  animator->current_animation = NULL;
  animator->animation_time = 0.0f;
}

void animator_do_animation(struct Animator* animator, struct Animation* animation) {
  animator->animation_time = 0.0f;
  animator->current_animation = animation;
}

void animator_update(struct Animator* animator, float delta_time) {
  if (animator->current_animation == NULL)
    return;
  animator_increase_animation_time(animator, delta_time);
  struct Map* current_pose = animator_calculate_current_animation_pose(animator);
  mat4 parent_transform = GLM_MAT4_IDENTITY_INIT;
  animator_apply_pose_to_joints(current_pose, animator->entity->root_joint, parent_transform);
  map_delete(current_pose);
  free(current_pose);
}

void animator_increase_animation_time(struct Animator* animator, float delta_time) {
  animator->animation_time += delta_time / 2.0f;
  if (animator->animation_time > animator->current_animation->length)
    animator->animation_time = fmodf(animator->animation_time, animator->current_animation->length);
}

struct Map* animator_calculate_current_animation_pose(struct Animator* animator) {
  struct KeyFrame *previous_frame = NULL, *next_frame = NULL;
  animator_get_previous_and_next_frames(animator, &previous_frame, &next_frame);
  float progression = animator_calculate_progression(animator, previous_frame, next_frame);
  return animator_interpolate_poses(previous_frame, next_frame, progression);
}

void animator_apply_pose_to_joints(struct Map* current_pose, struct Joint* joint, mat4 parent_transform) {
  mat4 current_local_transform = GLM_MAT4_ZERO_INIT;
  glm_mat4_copy(*(mat4*)map_get(current_pose, joint->name), current_local_transform);
  mat4 current_transform = GLM_MAT4_ZERO_INIT;
  glm_mat4_mul(parent_transform, current_local_transform, current_transform);

  for (int child_joint_num = 0; child_joint_num < array_list_size(joint->children); child_joint_num++) {
    struct Joint* child_joint = (struct Joint*)array_list_get(joint->children, child_joint_num);
    animator_apply_pose_to_joints(current_pose, child_joint, current_transform);
  }

  glm_mat4_mul(current_transform, joint->inverse_bind_transform, current_transform);
  glm_mat4_copy(current_transform, joint->animation_transform);
}

void animator_get_previous_and_next_frames(struct Animator* animator, struct KeyFrame** previous_frame, struct KeyFrame** next_frame) {
  struct ArrayList* all_frames = animator->current_animation->key_frames;
  *previous_frame = (struct KeyFrame*)array_list_get(all_frames, 0);
  *next_frame = (struct KeyFrame*)array_list_get(all_frames, 0);
  for (int frame_num = 1; frame_num < array_list_size(all_frames); frame_num++) {
    *next_frame = (struct KeyFrame*)array_list_get(all_frames, frame_num);
    if ((*next_frame)->time_step > animator->animation_time)
      break;
    *previous_frame = (struct KeyFrame*)array_list_get(all_frames, frame_num);
  }
}

float animator_calculate_progression(struct Animator* animator, struct KeyFrame* previous_frame, struct KeyFrame* next_frame) {
  float total_time = next_frame->time_step - previous_frame->time_step;
  float current_time = animator->animation_time - previous_frame->time_step;
  return current_time / total_time;
}

struct Map* animator_interpolate_poses(struct KeyFrame* previous_frame, struct KeyFrame* next_frame, float progression) {
  struct Map* current_pose = malloc(sizeof(struct Map));
  map_init(current_pose, sizeof(mat4));

  const char* joint_name = NULL;
  struct MapIter iter = map_iter();
  while ((joint_name = map_next(previous_frame->pose, &iter))) {
    struct JointTransform* previous_transform = (struct JointTransform*)map_get(previous_frame->pose, joint_name);
    struct JointTransform* next_transform = (struct JointTransform*)map_get(next_frame->pose, joint_name);
    struct JointTransform current_transform = joint_transform_interpolate(previous_transform, next_transform, progression);
    mat4 local_transform = GLM_MAT4_IDENTITY_INIT;
    joint_transform_get_local_transform(current_transform, local_transform);
    map_set(current_pose, joint_name, local_transform);
  }
  return current_pose;
}
