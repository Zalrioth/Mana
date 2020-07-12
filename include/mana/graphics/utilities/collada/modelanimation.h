#pragma once
#ifndef MODEL_ANIMATION
#define MODEL_ANIMATION

#include "mana/core/memoryallocator.h"
//
#include <cstorage/cstorage.h>
#include <ubermath/ubermath.h>

#include "mana/core/corecommon.h"
#include "mana/core/xmlnode.h"
#include "mana/graphics/entities/model.h"
#include "mana/graphics/utilities/mesh.h"

struct AnimationData {
  float length_seconds;
  struct ArrayList* key_frames;
};

static inline void animation_data_init(struct AnimationData* animation_data, float length_seconds, struct ArrayList* key_frames) {
  animation_data->length_seconds = length_seconds;
  animation_data->key_frames = key_frames;
}

struct KeyFrameData {
  float time;
  struct ArrayList* joint_transforms;
};

static inline void key_frame_data_init(struct KeyFrameData* key_frame_data, float time) {
  key_frame_data->joint_transforms = malloc(sizeof(struct ArrayList));
  array_list_init(key_frame_data->joint_transforms);
  key_frame_data->time = time;
}

struct JointTransformData {
  char* joint_name_id;
  mat4 joint_local_transform;
};

static inline void joint_transform_data_init(struct JointTransformData* joint_transform_data, char* joint_name_id, mat4 joint_local_transform) {
  joint_transform_data->joint_name_id = joint_name_id;
  joint_transform_data->joint_local_transform = joint_local_transform;
}

struct AnimationData* animation_extract_animation(struct XmlNode* animation_data, struct XmlNode* joint_hierarchy);
struct Vector* animation_get_key_times(struct XmlNode* animation_data);
struct ArrayList* animation_init_key_frames(struct Vector* times);
void animation_load_joint_transform(struct ArrayList* frames, struct XmlNode* joint_data, char* root_node_id);
void animation_process_transforms(char* joint_name, char* raw_data, struct ArrayList* key_frames, bool root);

#endif  // MODEL_ANIMATION
