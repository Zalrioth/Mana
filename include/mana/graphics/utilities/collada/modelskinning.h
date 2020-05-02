#pragma once
#ifndef MODEL_SKINNING_H
#define MODEL_SKINNING_H

#include "mana/core/memoryallocator.h"
//
#include <cstorage/cstorage.h>
#include <stdint.h>

#include "mana/core/corecommon.h"
#include "mana/core/xmlnode.h"

struct VertexSkinData {
  struct Vector* joint_ids;
  struct Vector* weights;
};

void vertex_skin_data_init(struct VertexSkinData* vertex_skin_data);
void vertex_skin_data_delete(struct VertexSkinData* vertex_skin_data);
void vertex_skin_data_add_joint_effect(struct VertexSkinData* vertex_skin_data, int joint_id, float weight);
void vertex_skin_data_limit_joint_number(struct VertexSkinData* vertex_skin_data, int max);
void vertex_skin_data_fill_empty_weights(struct VertexSkinData* vertex_skin_data, int max);
float vertex_skin_data_save_top_weights(struct VertexSkinData* vertex_skin_data, float* top_weights, int max);
void vertex_skin_data_refill_weight_list(struct VertexSkinData* vertex_skin_data, float* top_weights, int max, float total);
void vertex_skin_data_remove_excess_joint_ids(struct VertexSkinData* vertex_skin_data, int max);

struct SkinningData {
  struct Vector* joint_order;
  struct Vector* vertices_skin_data;
};

static inline void skinning_data_init(struct SkinningData* skinning_data, struct Vector* joint_order, struct Vector* vertices_skin_data) {
  skinning_data->joint_order = joint_order;
  skinning_data->vertices_skin_data = vertices_skin_data;
}

struct SkinningData* skin_loader_extract_skin_data(struct XmlNode* skinning_data, int max_weights);
struct Vector* skin_loader_load_joints_list(struct XmlNode* skinning_data);
struct Vector* skin_loader_load_weights(struct XmlNode* skinning_data);
struct Vector* skin_loader_get_effective_joints_counts(struct XmlNode* weights_data_node);
struct Vector* skin_loader_get_skin_data(int max_weights, struct XmlNode* weights_data_node, struct Vector* counts, struct Vector* weights);

#endif  // MODEL_SKINNING_H
