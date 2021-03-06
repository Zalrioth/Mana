#pragma once
#ifndef MODEL_SKELETON_H
#define MODEL_SKELETON_H

#include "mana/core/memoryallocator.h"
//
#include <cstorage/cstorage.h>
#include <ubermath/ubermath.h>

#include "xmlnode.h"

struct JointData {
  int index;
  char *name_id;
  mat4 bind_local_transform;
  struct ArrayList *children;
};

static inline void joint_data_init(struct JointData *joint_data, int index, char *name_id, mat4 bind_local_transform) {
  joint_data->index = index;
  joint_data->name_id = strdup(name_id);
  joint_data->bind_local_transform = bind_local_transform;
  joint_data->children = malloc(sizeof(struct ArrayList));
  array_list_init(joint_data->children);
}

struct SkeletonData {
  int joint_count;
  struct JointData *head_joint;
};

static inline void skeleton_data_init(struct SkeletonData *skeleton_data, int joint_count, struct JointData *head_joint) {
  skeleton_data->joint_count = joint_count;
  skeleton_data->head_joint = head_joint;
}

struct SkeletonData *skeleton_loader_extract_bone_data(struct XmlNode *visual_scene_node, struct Vector *bone_order);
struct JointData *skeleton_loader_load_joint_data(struct XmlNode *joint_node, struct Vector *bone_order, bool is_root, int *joint_count);
struct JointData *skeleton_loader_extract_main_joint_data(struct XmlNode *joint_node, struct Vector *bone_order, bool is_root, int *joint_count);
mat4 skeleton_loader_convert_data(char *matrix_data);

#endif  // MODEL_SKELETON_H
