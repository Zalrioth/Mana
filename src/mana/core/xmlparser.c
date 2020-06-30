#include "mana/core/xmlparser.h"

struct XmlNode* xml_parser_load_xml_file(char* xml_file_path) {
  int xml_file_length = 0;
  char* xml_file_data = read_file(xml_file_path, &xml_file_length);
  char* file_start = xml_file_data;

  if (strncmp(file_start, "<?", 2) == 0)
    file_start = strchr(file_start, '\n') + 1;
  char** scanner = &file_start;

  struct XmlNode* xml_node = xml_parser_load_node(scanner);

  free(xml_file_data);

  return xml_node;
}

struct XmlNode* xml_parser_load_node(char** scanner) {
  // Extract line
  char* line_end = strchr(*scanner, '\n');
  if (line_end == NULL)
    line_end = strchr(*scanner, '\0');
  size_t line_length = line_end - *scanner;
  if (line_length == 0)
    return NULL;
  char* line = malloc(sizeof(char) * (line_length + 1));
  strncpy(line, *scanner, line_length);
  line[line_length] = '\0';

  *scanner += line_length + 1;

  // Trim whitespace
  char* remove_whitespace_line = line;
  while (isspace((unsigned char)*remove_whitespace_line))
    remove_whitespace_line++;

  if (strncmp(remove_whitespace_line, "</", 2) == 0) {
    free(line);
    return NULL;
  }

  // Extract tag in line
  char* tag_end = strchr(remove_whitespace_line, '>');
  size_t tag_length = tag_end - (remove_whitespace_line + 1);
  char* tag = malloc(sizeof(char) * (tag_length + 1));
  strncpy(tag, remove_whitespace_line + 1, tag_length);
  tag[tag_length] = '\0';

  // Split tag elements
  struct ArrayList tag_parts = {0};
  array_list_init(&tag_parts);
  char* tag_part = strtok(tag, " ");
  while (tag_part != NULL) {
    array_list_add(&tag_parts, tag_part);
    tag_part = strtok(NULL, " ");
  }

  //printf("DAT: %s\n", remove_whitespace_line);
  //for (int tag_num = 0; tag_num < array_list_size(&tag_parts); tag_num++)
  //  printf("Tag num: %d is %s\n", tag_num, (char*)array_list_get(&tag_parts, tag_num));

  // Remove slash
  struct XmlNode* xml_node = calloc(1, sizeof(struct XmlNode));
  char* node_name = strdup((char*)array_list_get(&tag_parts, 0));
  char* check_slash = strchr(node_name, '/');
  if (check_slash != NULL)
    memmove(check_slash, check_slash + 1, strlen(check_slash));
  xml_node_init(xml_node, node_name);

  // Add attributes
  for (int tag_num = 0; tag_num < array_list_size(&tag_parts); tag_num++) {
    char* tag_text = (char*)array_list_get(&tag_parts, tag_num);
    size_t tag_text_length = strlen(tag_text);
    char* tag_contains_equal = strchr(tag_text, '=');
    if (tag_contains_equal != NULL) {
      size_t tag_equal_length = tag_contains_equal - tag_text;
      char* tag_equal_text = malloc(sizeof(char) * (tag_equal_length + 1));
      strncpy(tag_equal_text, tag_text, tag_equal_length);
      tag_equal_text[tag_equal_length] = '\0';

      //TODO: Add support for " " name tags like line 2242 of model
      char* tag_attr_start = tag_text + tag_equal_length + 2;
      char* tag_attr_end;
      // Null or empty tag value
      if (*tag_attr_start == '\0')
        tag_attr_end = tag_attr_start + 1;
      else
        tag_attr_end = strchr(tag_attr_start, '\"');
      size_t tag_value_length = tag_attr_end - tag_attr_start;
      char* tag_value = malloc(sizeof(char) * (tag_value_length + 1));
      strncpy(tag_value, tag_attr_start, tag_value_length);
      tag_value[tag_value_length] = '\0';

      xml_node_add_attribute(xml_node, tag_equal_text, tag_value);
      free(tag_equal_text);
    }
  }

  // Add data
  char* data_start = strchr(line, '>') + 1;
  //printf("%s\n", data_start);
  char* data_end = strchr(data_start, '<');
  if (data_end != NULL) {
    size_t data_length = data_end - data_start;
    char* data_value = malloc(sizeof(char) * (data_length + 1));
    strncpy(data_value, data_start, data_length);
    data_value[data_length] = '\0';

    xml_node_set_data(xml_node, data_value);
  }

  // Check if closing (start or end)
  char* closed_end = strstr(line, "</");
  char* closed_start = strstr(line, "/>");
  if (closed_end != NULL || closed_start != NULL) {
    free(line);
    free(tag);
    return xml_node;
  }

  // Recursive all nodes
  struct XmlNode* xml_child = NULL;
  while ((xml_child = xml_parser_load_node(scanner)) != NULL)
    xml_node_add_child(xml_node, xml_child);

  // Done with current node
  free(line);
  free(tag);
  return xml_node;
}

void xml_parser_delete(struct XmlNode* xml_node) {
  if (xml_node->child_nodes != NULL) {
    const char* key;
    struct MapIter iter = map_iter();
    while ((key = map_next(xml_node->child_nodes, &iter))) {
      struct ArrayList** child_list_pointer = (struct ArrayList**)map_get(xml_node->child_nodes, key);
      if (child_list_pointer != NULL) {
        for (int child_num = 0; child_num < array_list_size(*child_list_pointer); child_num++)
          xml_parser_delete((struct XmlNode*)array_list_get(*child_list_pointer, child_num));
      }
    }
  }
  xml_node_delete(xml_node);
  free(xml_node);
}
