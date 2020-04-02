#pragma once
#ifndef MODEL_SKELETON_H
#define MODEL_SKELETON_H

#include "mana/core/memoryallocator.h"
//
#include <cglm/cglm.h>
#include <cstorage/cstorage.h>

#include "mana/core/xmlnode.h"

struct JointData {
  int index;
  char *name_id;
  mat4 bind_local_transform;
  struct ArrayList *children;
};

void joint_data_init(struct JointData *joint_data, int index, char *name_id, mat4 bind_local_transform);
void joint_data_delete(struct JointData *joint_data);
void joint_data_add_child(struct JointData *joint_data, struct JointData *child);

struct SkeletonData {
  int joint_count;
  struct JointData *head_joint;
};

void skeleton_data_init(struct SkeletonData *skeleton_data, int joint_count, struct JointData *head_joint);

struct SkeletonData *skeleton_loader_extract_bone_data(struct XmlNode *visual_scene_node, struct Vector *bone_order);
struct JointData *skeleton_loader_load_joint_data(struct XmlNode *joint_node, struct Vector *bone_order, bool is_root, int *joint_count);
struct JointData *skeleton_loader_extract_main_joint_data(struct XmlNode *joint_node, struct Vector *bone_order, bool is_root, int *joint_count);
void skeleton_loader_convert_data(mat4 mat_dest, char *matrix_data);

#endif  // MODEL_SKELETON_H
