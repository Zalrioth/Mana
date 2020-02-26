#include "mana/core/xmlparser.h"

struct XmlNode* xml_parser_load_xml_file(char* xml_file_path) {
  int xml_file_length = 0;
  char* xml_file_data = read_file(xml_file_path, &xml_file_length);
  char** scanner = &xml_file_data;

  struct XmlNode* xml_node = xml_parser_load_node(scanner);

  //char* cur_line = xml_file_contents;
  //while (cur_line) {
  //  char* nextLine = strchr(cur_line, '\n');
  //
  //  if (nextLine)
  //    *nextLine = '\0';
  //
  //  printf("curLine=[%s]\n", cur_line);
  //
  //  if (nextLine)
  //    *nextLine = '\n';
  //
  //  cur_line = nextLine ? (nextLine + 1) : NULL;
  //}

  free(xml_file_data);

  return NULL;
}

struct XmlNode* xml_parser_load_node(char** xml_file_data) {
  // Extract line
  char* line_end = strchr(*xml_file_data, '\n');
  size_t line_length = (line_end - *xml_file_data) + 1;
  char* line = malloc(sizeof(char) * line_length);
  strncpy(line, *xml_file_data, line_length);
  line[line_length - 1] = '\0';

  *xml_file_data += line_length;

  printf("Raw: %s\n", line);

  // Trim whitespace
  char* remove_whitespace_line = line;
  while (isspace((unsigned char)*remove_whitespace_line))
    remove_whitespace_line++;

  printf("WS: %s\n", remove_whitespace_line);

  if (strncmp(remove_whitespace_line, "</", 2) == 0)
    return NULL;

  // Extract tag in line
  char* tag_end = strchr(remove_whitespace_line, '>');
  size_t tag_length = (tag_end - remove_whitespace_line) + 1;
  char* tag = calloc(1, sizeof(char) * tag_length);
  strncpy(tag, remove_whitespace_line, tag_length);
  tag[tag_length] = '\0';

  //printf("%s\n", tag);

  // Split tag elements
  struct ArrayList tag_parts = {0};
  array_list_init(&tag_parts);
  char* tag_part = strtok(tag, " ");
  while (tag_part != NULL) {
    array_list_add(&tag_parts, tag_part);
    tag_part = strtok(NULL, " ");
  }

  //for (int tag_num = 0; tag_num < array_list_size(&tag_parts); tag_num++)
  //  printf("Tag num: %d is %s\n", tag_num, (char*)array_list_get(&tag_parts, tag_num));

  // Remove slash
  struct XmlNode* xml_node = calloc(1, sizeof(struct XmlNode));
  char* node_name = strdup((char*)array_list_get(&tag_parts, 0));
  char* check_slash = strchr(node_name, '/');
  if (check_slash != NULL)
    memmove(check_slash, check_slash + 1, strlen(node_name) - strlen(check_slash));
  xml_node_init(xml_node, node_name);

  // Add attributes//
  for (int tag_num = 0; tag_num < array_list_size(&tag_parts); tag_num++) {
    char* tag_text = (char*)array_list_get(&tag_parts, tag_num);
    size_t tag_text_length = strlen(tag_text);
    char* tag_contains_equal = strchr(tag_text, '=');
    if (tag_contains_equal != NULL) {
      size_t tag_equal_length = (tag_contains_equal - tag_text) + 1;
      char* tag_equal_text = malloc(sizeof(char) * tag_equal_length);
      strncpy(tag_equal_text, tag_text, tag_equal_length);
      tag_equal_text[tag_equal_length - 1] = '\0';

      char* tag_attr_start = tag_text + tag_equal_length + 1;
      char* tag_attr_end = strchr(tag_attr_start, '\"');
      size_t tag_value_length = ((tag_attr_end - tag_attr_start) + 1);
      char* tag_value = malloc(sizeof(char) * tag_value_length);
      strncpy(tag_value, tag_attr_start, tag_value_length);
      tag_value[tag_value_length - 1] = '\0';

      xml_node_add_attribute(xml_node, tag_equal_text, tag_value);
    }
  }

  // Add data
  char* data_start = strchr(line, '>');
  //printf("%s\n", data_start);
  char* data_end = strchr(data_start, '<');
  if (data_end != NULL) {
    size_t data_length = ((data_end - data_start) + 1);
    char* data_value = malloc(sizeof(char) * data_length);
    strncpy(data_value, data_start, data_length);
    data_value[data_length - 1] = '\0';
    xml_node_set_data(xml_node, data_value);
  }

  // Check if closing (start or end)
  char* closed_end = strstr(line, "</");
  char* closed_start = strstr(line, "/>");
  if (closed_end != NULL || closed_start != NULL)
    return xml_node;

  // Recursive all nodes
  struct XmlNode* xml_child = NULL;
  while ((xml_child = xml_parser_load_node(xml_file_data)) != NULL) {
    printf("HEEEERE: %s\n", remove_whitespace_line);
    printf("NODE: %s\n", xml_child->name);
    xml_node_add_child(xml_node, xml_child);
  }

  // Done with current node
  return xml_node;
}
