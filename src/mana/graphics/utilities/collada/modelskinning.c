#include "mana/graphics/utilities/collada/modelskinning.h"

void vertex_skin_data_init(struct VertexSkinData* vertex_skin_data) {
  vertex_skin_data->joint_ids = malloc(sizeof(struct Vector));
  vertex_skin_data->weights = malloc(sizeof(struct Vector));

  vector_init(vertex_skin_data->joint_ids, sizeof(int));
  vector_init(vertex_skin_data->weights, sizeof(float));
}

void vertex_skin_data_delete(struct VertexSkinData* vertex_skin_data) {
}

void vertex_skin_data_add_joint_effect(struct VertexSkinData* vertex_skin_data, int joint_id, float weight) {
  for (int weight_num = 0; weight_num < vector_size(vertex_skin_data->weights); weight_num++) {
    if (weight > *(float*)vector_get(vertex_skin_data->weights, weight_num)) {
      vector_insert(vertex_skin_data->joint_ids, weight_num, &joint_id);
      vector_insert(vertex_skin_data->weights, weight_num, &weight);
      return;
    }
  }
  vector_push_back(vertex_skin_data->joint_ids, &joint_id);
  vector_push_back(vertex_skin_data->weights, &weight);
}

void vertex_skin_data_limit_joint_number(struct VertexSkinData* vertex_skin_data, int max) {
  if (vector_size(vertex_skin_data->joint_ids) > max) {
    float* top_weights = malloc(sizeof(float) * max);
    float total = vertex_skin_data_save_top_weights(vertex_skin_data, top_weights, max);
    vertex_skin_data_refill_weight_list(vertex_skin_data, top_weights, max, total);
    vertex_skin_data_remove_excess_joint_ids(vertex_skin_data, max);
  } else if (vector_size(vertex_skin_data->joint_ids) < max)
    vertex_skin_data_fill_empty_weights(vertex_skin_data, max);
}

void vertex_skin_data_fill_empty_weights(struct VertexSkinData* vertex_skin_data, int max) {
  int zero_int = 0;
  float zero_float = 0.0f;
  while (vector_size(vertex_skin_data->joint_ids) < max) {
    vector_push_back(vertex_skin_data->joint_ids, &zero_int);
    vector_push_back(vertex_skin_data->weights, &zero_float);
  }
}

float vertex_skin_data_save_top_weights(struct VertexSkinData* vertex_skin_data, float* top_weights, int max) {
  float total = 0.0f;
  for (int top_weight_num = 0; top_weight_num < max; top_weight_num++) {
    top_weights[top_weight_num] = *(float*)vector_get(vertex_skin_data->weights, top_weight_num);
    total += top_weights[top_weight_num];
  }
  return total;
}

void vertex_skin_data_refill_weight_list(struct VertexSkinData* vertex_skin_data, float* top_weights, int max, float total) {
  vector_clear(vertex_skin_data->weights);
  for (int top_weight_num = 0; top_weight_num < max; top_weight_num++) {
    float new_weight = min(top_weights[top_weight_num] / total, 1.0f);
    vector_push_back(vertex_skin_data->weights, &new_weight);
  }
}

void vertex_skin_data_remove_excess_joint_ids(struct VertexSkinData* vertex_skin_data, int max) {
  while (vector_size(vertex_skin_data->joint_ids) > max)
    vector_remove(vertex_skin_data->joint_ids, vector_size(vertex_skin_data->joint_ids) - 1);
}

struct SkinningData* skin_loader_extract_skin_data(struct XmlNode* skinning_data, int max_weights) {
  skinning_data = xml_node_get_child(xml_node_get_child(skinning_data, "controller"), "skin");
  struct Vector* joints_list = skin_loader_load_joints_list(skinning_data);
  struct Vector* weights = skin_loader_load_weights(skinning_data);
  struct XmlNode* weights_data_node = xml_node_get_child(skinning_data, "vertex_weights");
  struct Vector* effector_joint_counts = skin_loader_get_effective_joints_counts(weights_data_node);
  /////////////////////////
  struct Vector* vertex_weights = skin_loader_get_skin_data(max_weights, weights_data_node, effector_joint_counts, weights);
  struct SkinningData* parsed_skinning_data = malloc(sizeof(struct SkinningData));
  skinning_data_init(parsed_skinning_data, joints_list, vertex_weights);
  return parsed_skinning_data;
}

struct Vector* skin_loader_load_joints_list(struct XmlNode* skinning_data) {
  struct XmlNode* input_node = xml_node_get_child(skinning_data, "vertex_weights");
  char* joint_data_id = xml_node_get_attribute(xml_node_get_child_with_attribute(input_node, "input", "semantic", "JOINT"), "source") + 1;
  char* weights_data_id = xml_node_get_attribute(xml_node_get_child_with_attribute(input_node, "input", "semantic", "WEIGHT"), "source") + 1;
  struct XmlNode* joint_node = xml_node_get_child(xml_node_get_child_with_attribute(skinning_data, "source", "id", joint_data_id), "Name_array");

  char* raw_data = strdup(xml_node_get_data(joint_node));
  struct Vector* joints_list = malloc(sizeof(struct Vector));
  vector_init(joints_list, sizeof(char*));
  char* raw_part = strtok(raw_data, " ");
  while (raw_part != NULL) {
    // TODO: These will need to be freed
    char* parsed_joint = strdup(raw_part);
    vector_push_back(joints_list, &parsed_joint);
    raw_part = strtok(NULL, " ");
  }
  free(raw_data);
  return joints_list;
}

struct Vector* skin_loader_load_weights(struct XmlNode* skinning_data) {
  struct XmlNode* input_node = xml_node_get_child(skinning_data, "vertex_weights");
  char* weights_data_id = xml_node_get_attribute(xml_node_get_child_with_attribute(input_node, "input", "semantic", "WEIGHT"), "source") + 1;
  struct XmlNode* weights_node = xml_node_get_child(xml_node_get_child_with_attribute(skinning_data, "source", "id", weights_data_id), "float_array");

  char* raw_data = strdup(xml_node_get_data(weights_node));
  struct Vector* weights = malloc(sizeof(struct Vector));
  vector_init(weights, sizeof(float));
  char* raw_part = strtok(raw_data, " ");
  while (raw_part != NULL) {
    float parsed_weight = atof(raw_part);
    vector_push_back(weights, &parsed_weight);
    raw_part = strtok(NULL, " ");
  }
  free(raw_data);
  return weights;
}

struct Vector* skin_loader_get_effective_joints_counts(struct XmlNode* weights_data_node) {
  char* raw_data = strdup(xml_node_get_data(xml_node_get_child(weights_data_node, "vcount")));
  struct Vector* counts = malloc(sizeof(struct Vector));
  vector_init(counts, sizeof(int));
  char* raw_part = strtok(raw_data, " ");
  while (raw_part != NULL) {
    int parsed_count = atoi(raw_part);
    vector_push_back(counts, &parsed_count);
    raw_part = strtok(NULL, " ");
  }
  free(raw_data);
  return counts;
}

struct Vector* skin_loader_get_skin_data(int max_weights, struct XmlNode* weights_data_node, struct Vector* counts, struct Vector* weights) {
  char* raw_data = strdup(xml_node_get_data(xml_node_get_child(weights_data_node, "v")));
  struct Vector* skinning_data = malloc(sizeof(struct Vector));
  vector_init(skinning_data, sizeof(struct VertexSkinData));
  char* raw_part = strtok(raw_data, " ");
  for (int count = 0; count < vector_size(counts); count++) {
    struct VertexSkinData* skin_data = malloc(sizeof(struct VertexSkinData));
    vertex_skin_data_init(skin_data);
    for (int loop_num = 0; loop_num < *(int*)vector_get(counts, count); loop_num++) {
      int joint_id = atoi(raw_part);
      raw_part = strtok(NULL, " ");
      int weight_id = atoi(raw_part);
      raw_part = strtok(NULL, " ");
      //printf("Count: %d\n", parsed_count);
      vertex_skin_data_add_joint_effect(skin_data, joint_id, *(float*)vector_get(weights, weight_id));
    }
    vertex_skin_data_limit_joint_number(skin_data, max_weights);
    vector_push_back(skinning_data, skin_data);
  }
  free(raw_data);
  return skinning_data;
}
