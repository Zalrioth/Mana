#include "mana/graphics/entities/model.h"

void model_init(struct Model* model, char* path, int max_weights) {
  model->mesh = malloc(sizeof(struct Mesh));
  //model->texture = malloc(sizeof(struct Texture));

  struct XmlNode* node = xml_parser_load_xml_file(path);
  struct SkinningData* skinning_data = skin_loader_extract_skin_data(xml_node_get_child(node, "library_controllers"), max_weights);
  // SkeletonData
  // MeshData
}