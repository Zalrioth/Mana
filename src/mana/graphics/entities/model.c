#include "mana/graphics/entities/model.h"

void model_init(struct Model* model, char* path, bool animated) {
  xml_parser_load_xml_file(path);
}