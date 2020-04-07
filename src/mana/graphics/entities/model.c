#include "mana/graphics/entities/model.h"

void model_init(struct Model* model, struct VulkanRenderer* vulkan_renderer, char* mode_path, char* texture_path, int max_weights) {
  struct XmlNode* node = xml_parser_load_xml_file(mode_path);
  struct SkinningData* skinning_data = skin_loader_extract_skin_data(xml_node_get_child(node, "library_controllers"), max_weights);

  model->joints = skeleton_loader_extract_bone_data(xml_node_get_child(node, "library_visual_scenes"), skinning_data->joint_order);
  //model->mesh = geometry_loader_extract_model_data(xml_node_get_child(node, "library_geometries"), skinning_data->vertices_skin_data);
  model->texture = malloc(sizeof(struct Texture));
  texture_init(model->texture, vulkan_renderer, texture_path);

  // If animated
  //XmlNode node = XmlParser.loadXmlFile(colladaFile);
  //XmlNode animNode = node.getChild("library_animations");
  //XmlNode jointsNode = node.getChild("library_visual_scenes");
  //AnimationLoader loader = new AnimationLoader(animNode, jointsNode);
  //AnimationData animData = loader.extractAnimation();
  //return animData;
}