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
  struct Map* child_nodes;  // Children are stored in an arraylist due to repeated tags
};

static inline void xml_node_init(struct XmlNode* xml_node, char* name);
static inline void xml_node_delete(struct XmlNode* xml_node);
static inline char* xml_node_get_attribute(struct XmlNode* xml_node, char* attr);
static inline struct XmlNode* xml_node_get_child(struct XmlNode* xml_node, char* child_name);
static inline struct XmlNode* xml_node_get_child_with_attribute(struct XmlNode* xml_node, char* child_name, char* attr, char* value);
static inline struct ArrayList* xml_node_get_children(struct XmlNode* xml_node, char* name);
static inline void xml_node_add_attribute(struct XmlNode* xml_node, char* attr, char* value);
static inline void xml_node_add_child(struct XmlNode* xml_node, struct XmlNode* child);
static inline char* xml_node_get_data(struct XmlNode* xml_node);
static inline void xml_node_set_data(struct XmlNode* xml_node, char* data);

static inline void xml_node_init(struct XmlNode* xml_node, char* name) {
  xml_node->name = name;
}

static inline void xml_node_delete(struct XmlNode* xml_node) {
  free(xml_node->name);
  if (xml_node->attributes != NULL) {
    const char* attributes_key;
    struct MapIter attributes_iter = map_iter();
    while ((attributes_key = map_next(xml_node->attributes, &attributes_iter)))
      free(*((char**)map_get(xml_node->attributes, attributes_key)));
    map_delete(xml_node->attributes);
    free(xml_node->attributes);
  }
  free(xml_node->data);
  if (xml_node->child_nodes != NULL) {
    const char* child_node_key;
    struct MapIter child_node_iter = map_iter();
    while ((child_node_key = map_next(xml_node->child_nodes, &child_node_iter))) {
      struct ArrayList** child_list_pointer = (struct ArrayList**)map_get(xml_node->child_nodes, child_node_key);
      array_list_delete(*child_list_pointer);
      free(*child_list_pointer);
    }
    map_delete(xml_node->child_nodes);
  }
  free(xml_node->child_nodes);
}

static inline char* xml_node_get_attribute(struct XmlNode* xml_node, char* attr) {
  if (xml_node->attributes != NULL)
    return *(char**)map_get(xml_node->attributes, attr);
  else
    return NULL;
}

static inline struct XmlNode* xml_node_get_child(struct XmlNode* xml_node, char* child_name) {
  if (xml_node->child_nodes != NULL) {
    struct ArrayList** nodes = (struct ArrayList**)map_get(xml_node->child_nodes, child_name);
    if (nodes != NULL && !array_list_empty(*nodes))
      return array_list_get(*nodes, 0);
  }
  return NULL;
}

static inline struct XmlNode* xml_node_get_child_with_attribute(struct XmlNode* xml_node, char* child_name, char* attr, char* value) {
  struct ArrayList* children = xml_node_get_children(xml_node, child_name);
  if (children == NULL || array_list_empty(children))
    return NULL;

  for (int child_num = 0; child_num < array_list_size(children); child_num++) {
    struct XmlNode* child = (struct XmlNode*)array_list_get(children, child_num);
    char* val = xml_node_get_attribute(child, attr);
    if (strcmp(value, val) == 0)
      return child;
  }

  return NULL;
}

static inline struct ArrayList* xml_node_get_children(struct XmlNode* xml_node, char* name) {
  if (xml_node->child_nodes != NULL) {
    struct ArrayList** children = (struct ArrayList**)map_get(xml_node->child_nodes, name);
    if (children != NULL)
      return *children;
  }
  return NULL;
}

static inline void xml_node_add_attribute(struct XmlNode* xml_node, char* attr, char* value) {
  if (xml_node->attributes == NULL) {
    xml_node->attributes = malloc(sizeof(struct Map));
    map_init(xml_node->attributes, sizeof(char*));
  }
  map_set(xml_node->attributes, attr, &value);
}

static inline void xml_node_add_child(struct XmlNode* xml_node, struct XmlNode* child) {
  if (xml_node->child_nodes == NULL) {
    xml_node->child_nodes = malloc(sizeof(struct Map));
    map_init(xml_node->child_nodes, sizeof(struct ArrayList*));
  }
  struct ArrayList** list = (struct ArrayList**)map_get(xml_node->child_nodes, child->name);
  if (list == NULL) {
    struct ArrayList* new_list = malloc(sizeof(struct ArrayList));
    array_list_init(new_list);
    map_set(xml_node->child_nodes, child->name, &new_list);
    array_list_add(new_list, child);
  } else
    array_list_add(*list, child);
}

static inline char* xml_node_get_data(struct XmlNode* xml_node) {
  return xml_node->data;
}

static inline void xml_node_set_data(struct XmlNode* xml_node, char* data) {
  xml_node->data = data;
}

#endif  // XML_NODE_H
