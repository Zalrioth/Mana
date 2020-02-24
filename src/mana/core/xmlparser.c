#include "mana/core/xmlparser.h"

struct XmlNode* xml_parser_init(char* xml_file_path) {
  int xml_file_length = 0;
  char* xml_file_data = read_file(xml_file_path, &xml_file_length);

  struct XmlNode* xml_node = xml_node_load(xml_file_data);

  free(xml_file_data);
}

struct XmlNode* xml_node_load(char* xml_file_contentst) {
}

void xml_node_add_data() {
}
