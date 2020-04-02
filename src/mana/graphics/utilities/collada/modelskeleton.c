#include "mana/graphics/utilities/collada/modelskeleton.h"

void joint_data_init(struct JointData *joint_data, int index, char *name_id, mat4 bind_local_transform) {
  joint_data->index = index;
  // NOTE: Might need to be duplicated
  joint_data->name_id = name_id;
  glm_mat4_copy(bind_local_transform, joint_data->bind_local_transform);
  joint_data->children = malloc(sizeof(struct ArrayList));
  array_list_init(joint_data->children);
}

void joint_data_delete(struct JointData *joint_data) {
  // TODO: Clean up
}

void joint_data_add_child(struct JointData *joint_data, struct JointData *child) {
  array_list_add(joint_data->children, child);
}

void skeleton_data_init(struct SkeletonData *skeleton_data, int joint_count, struct JointData *head_joint) {
  skeleton_data->joint_count = joint_count;
  skeleton_data->head_joint = head_joint;
}

struct SkeletonData *skeleton_loader_extract_bone_data(struct XmlNode *visual_scene_node, struct Vector *bone_order) {
  struct XmlNode *armature_data = xml_node_get_child_with_attribute(xml_node_get_child(visual_scene_node, "visual_scene"), "node", "id", "Armature");
  struct XmlNode *head_node = xml_node_get_child(armature_data, "node");

  int joint_count = 0;

  struct JointData *head_joint = skeleton_loader_load_joint_data(head_node, bone_order, true, &joint_count);
  struct SkeletonData *skeleton_data = malloc(sizeof(struct SkeletonData));
  skeleton_data_init(skeleton_data, joint_count, head_joint);
  return skeleton_data;
}

struct JointData *skeleton_loader_load_joint_data(struct XmlNode *joint_node, struct Vector *bone_order, bool is_root, int *joint_count) {
  struct JointData *joint = skeleton_loader_extract_main_joint_data(joint_node, bone_order, is_root, joint_count);
  struct ArrayList *joint_node_children = xml_node_get_children(joint_node, "node");
  if (joint_node_children != NULL) {
    for (int child_num = 0; child_num < array_list_size(joint_node_children); child_num++) {
      struct XmlNode *joint_data = (struct XmlNode *)array_list_get(joint_node_children, child_num);
      joint_data_add_child(joint, skeleton_loader_load_joint_data(joint_data, bone_order, false, joint_count));
    }
  }

  return joint;
}

struct JointData *skeleton_loader_extract_main_joint_data(struct XmlNode *joint_node, struct Vector *bone_order, bool is_root, int *joint_count) {
  char *name_id = xml_node_get_attribute(joint_node, "id");
  int index = -1;

  for (int bone_num = 0; bone_num < vector_size(bone_order); bone_num++) {
    char **bone_id = (char **)vector_get(bone_order, bone_num);
    if (strcmp(name_id, *bone_id) == 0) {
      index = bone_num;
      break;
    }
  }

  char *matrix_data = xml_node_get_data(xml_node_get_child(joint_node, "matrix"));
  mat4 matrix;
  glm_mat4_identity(matrix);
  skeleton_loader_convert_data(matrix, matrix_data);
  glm_mat4_transpose(matrix);
  if (is_root) {
    mat4 correction;
    glm_mat4_identity(correction);
    glm_rotate(correction, glm_rad(-90.0f), (vec3){1.0f, 0.0f, 0.0f});
    glm_mat4_mul(correction, matrix, matrix);
  }
  (*joint_count)++;
  struct JointData *joint_data = malloc(sizeof(struct JointData));
  joint_data_init(joint_data, index, name_id, matrix);
  return joint_data;
}

void skeleton_loader_convert_data(mat4 mat_dest, char *matrix_data) {
  char *raw_data = strdup(matrix_data);
  char *raw_part = strtok(raw_data, " ");
  int column = 0, row = 0;
  while (raw_part != NULL) {
    float parsed_weight = strtof(raw_part, NULL);
    mat_dest[column][row++] = parsed_weight;
    if (row == 4)
      column++, row = 0;

    raw_part = strtok(NULL, " ");
  }
  free(raw_data);
}
