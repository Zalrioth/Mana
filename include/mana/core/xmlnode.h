#pragma once
#ifndef XML_NODE_H
#define XML_NODE_H

#include "mana/core/memoryallocator.h"
//
#include <cstorage/cstorage.h>

struct XmlNode {
  char* name;
  struct Map* attributes;
  char* data;
  struct Map* child_nodes;
};

static inline xml_node_init(struct XmlNode* xml_node, char* name) {
  xml_node->name = strdup(name);
}

static inline xml_node_delete(struct XmlNode* xml_node) {
  free(xml_node->name);
  map_delete(xml_node->attributes);
  free(xml_node->attributes);
  free(xml_node->data);
}

char* xml_node_get_attribute(struct XmlNode* xml_node, char* attr) {
  if (xml_node->attributes != NULL)
    return map_get(&xml_node->attributes, attr);
  else
    return NULL;
}

struct XmlNode* xml_node_get_child(struct XmlNode* xml_node, char* child_name) {
  if (xml_node->child_nodes != NULL) {
    struct ArrayList* nodes = (struct ArrayList*)map_get(xml_node->child_nodes, child_name);
    if (nodes != NULL && !array_list_empty(nodes))
      return array_list_get(nodes, 0);
  }
  return NULL;
}

struct XmlNode* xml_node_get_child_with_attribute(struct XmlNode* xml_node, char* child_name, char* attr, char* value) {
  struct ArrayList* children = xml_node_get_children(xml_node, child_name);
  if (children == NULL || array_list_empty(children))
    return NULL;

  for (int child_num = 0; child_num < array_list_size(children); child_num++) {
    struct XmlNode* child = (struct XmlNode*)array_list_get(children, child_num);
    char* val = xml_node_get_attribute(child, attr);
    if (strcmp(value, val))
      return child;
  }

  return NULL;
}

struct ArrayList* xml_node_get_children(struct XmlNode* xml_node, char* name) {
  if (xml_node->child_nodes != NULL) {
    struct ArrayList* children = (struct ArrayList*)map_get(xml_node->child_nodes, name);
    if (children != NULL)
      return children;
  }
  return NULL;
}

void xml_node_add_attribute(struct XmlNode* xml_node, char* attr, char* value) {
  if (xml_node->attributes == NULL)
    map_init(xml_node->attributes, sizeof(char*));
  map_set(xml_node->attributes, attr, value);
}

void xml_node_add_child(struct XmlNode* xml_node, struct XmlNode* child) {
  if (xml_node->child_nodes == NULL)
    map_init(xml_node->child_nodes, sizeof(struct ArrayList*));
  struct ArrayList* list = (struct ArrayList*)map_get(xml_node->child_nodes, child->name);
  if (list == NULL) {
    struct ArrayList* list = calloc(1, sizeof(struct ArrayList*));
    array_list_init(list);
    map_set(xml_node->child_nodes, child->name, list);
  }
  array_list_add(list, child);
}

static inline xml_node_set_data(struct XmlNode* xml_node, char* data) {
  xml_node->data = strdup(data);
}

#endif  // XML_NODE_H
