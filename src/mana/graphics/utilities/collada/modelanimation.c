#include "mana/graphics/utilities/collada/modelanimation.h"

struct AnimationData* animation_extract_animation(struct XmlNode* animation_data, struct XmlNode* joint_hierarchy) {
  char* root_node = xml_node_get_attribute(xml_node_get_child(xml_node_get_child_with_attribute(xml_node_get_child(joint_hierarchy, "visual_scene"), "node", "id", "Armature"), "node"), "id");
  struct Vector* times = animation_get_key_times(animation_data);
  float duration = *(float*)vector_get(times, vector_size(times) - 1);
  struct ArrayList* key_frames = animation_init_key_frames(times);
  struct ArrayList* animation_node = xml_node_get_children(animation_data, "animation");
  for (int joint_node_num = 0; joint_node_num < array_list_size(animation_node); joint_node_num++)
    animation_load_joint_transform(key_frames, (struct XmlNode*)array_list_get(animation_node, joint_node_num), root_node);
  struct AnimationData* anim_data = malloc(sizeof(struct AnimationData));
  animation_data_init(anim_data, duration, key_frames);
  return anim_data;
}

struct Vector* animation_get_key_times(struct XmlNode* animation_data) {
  struct XmlNode* time_data = xml_node_get_child(xml_node_get_child(xml_node_get_child(animation_data, "animation"), "source"), "float_array");
  char* raw_times = strdup(xml_node_get_data(time_data));
  struct Vector* times = malloc(sizeof(struct Vector));
  vector_init(times, sizeof(float));
  char* raw_part = strtok(raw_times, " ");
  while (raw_part != NULL) {
    float time = atof(raw_part);
    vector_push_back(times, &time);
    raw_part = strtok(NULL, " ");
  }
  free(raw_times);
  return times;
}

struct ArrayList* animation_init_key_frames(struct Vector* times) {
  struct ArrayList* frames = malloc(sizeof(struct ArrayList));
  array_list_init(frames);
  for (int frame_num = 0; frame_num < vector_size(times); frame_num++) {
    struct KeyFrameData* key_frame_data = malloc(sizeof(struct KeyFrameData));
    key_frame_data_init(key_frame_data, *(float*)vector_get(times, frame_num));
    array_list_add(frames, key_frame_data);
  }
  return frames;
}

void animation_load_joint_transform(struct ArrayList* frames, struct XmlNode* joint_data, char* root_node_id) {
  char* joint_name_raw = xml_node_get_attribute(xml_node_get_child(joint_data, "channel"), "target");
  char* slash_pos = strchr(joint_name_raw, '/');
  int joint_name_length = slash_pos - joint_name_raw;
  char* joint_name_id = malloc(sizeof(char) * (joint_name_length + 1));
  memcpy(joint_name_id, joint_name_raw, joint_name_length);
  joint_name_id[joint_name_length] = '\0';
  char* data_id = xml_node_get_attribute(xml_node_get_child_with_attribute(xml_node_get_child(joint_data, "sampler"), "input", "semantic", "OUTPUT"), "source") + 1;
  struct XmlNode* transform_data = xml_node_get_child_with_attribute(joint_data, "source", "id", data_id);
  char* raw_data = xml_node_get_data(xml_node_get_child(transform_data, "float_array"));
  animation_process_transforms(joint_name_id, raw_data, frames, strcmp(joint_name_id, root_node_id) == 0);
}

void animation_process_transforms(char* joint_name, char* raw_data, struct ArrayList* key_frames, bool root) {
  char* raw_part = strtok(raw_data, " ");
  for (int key_frame_num = 0; key_frame_num < array_list_size(key_frames); key_frame_num++) {
    mat4 transform = GLM_MAT4_ZERO_INIT;
    for (int matrix_value = 0; matrix_value < 4; matrix_value++) {
      float m0 = atof(raw_part);
      raw_part = strtok(NULL, " ");
      float m1 = atof(raw_part);
      raw_part = strtok(NULL, " ");
      float m2 = atof(raw_part);
      raw_part = strtok(NULL, " ");
      float m3 = atof(raw_part);
      raw_part = strtok(NULL, " ");
      vec4 matrix_slice = {m0, m1, m2, m3};
      glm_vec4_copy(matrix_slice, transform[matrix_value]);
    }
    glm_mat4_transpose(transform);
    if (root) {
      mat4 correction = GLM_MAT4_IDENTITY_INIT;
      glm_rotate(correction, glm_rad(-90.0f), (vec3){1.0f, 0.0f, 0.0f});
      glm_mat4_mul(correction, transform, transform);
    }
    struct JointTransformData* joint_transform_data = malloc(sizeof(struct JointTransformData));
    joint_transform_data_init(joint_transform_data, joint_name, transform);
    array_list_add(((struct KeyFrameData*)array_list_get(key_frames, key_frame_num))->joint_transforms, joint_transform_data);
  }
}
