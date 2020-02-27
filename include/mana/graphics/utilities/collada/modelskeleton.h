#pragma once
#ifndef MODEL_SKELETON_H
#define MODEL_SKELETON_H

struct JointData {
  int index;
  char *name_id;
  mat4 bind_local_transform;
  struct ArrayList *children;
};

struct SkeletonData {
  int joint_count;
  struct JointData *head_joint;
};

#endif  // MODEL_SKELETON_H
