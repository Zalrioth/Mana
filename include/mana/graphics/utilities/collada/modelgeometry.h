#pragma once
#ifndef MODEL_GEOMETRY_H
#define MODEL_GEOMETRY_H

#include "mana/core/memoryallocator.h"
//
#include <cstorage/cstorage.h>

#include "mana/core/corecommon.h"
#include "mana/core/xmlnode.h"

struct Mesh* geometry_loader_extract_model_data(struct XmlNode* geometry_node, struct Vector* vertex_weights) {
  struct XmlNode* mesh_data = xml_node_get_child(xml_node_get_child(geometry_node, "geometry"), "mesh");
  geomtry_leader_read_raw_data(mesh_data);
}

void geomtry_leader_read_raw_data(struct XmlNode* mesh_data) {
  geometry_loader_read_positions(mesh_data);
  geometry_loader_read_normals(mesh_data);
}

void geometry_loader_read_positions(struct XmlNode* mesh_data) {
  char* positions_id = xml_node_get_attribute(xml_node_get_child(xml_node_get_child(mesh_data, "vertices"), "input"), "source") + 1;
  struct XmlNode* positions_data = xml_node_get_child(xml_node_get_child_with_attribute(mesh_data, "source", "id", "positionsId"), "float_array");
  int count = atoi(xml_node_get_attribute(positions_data, "count"));

  char* raw_data = strdup(xml_node_get_data(positions_data));
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

void geometry_loader_read_normals(struct XmlNode* mesh_data) {
}

#endif  // MODEL_GEOMETRY_H
