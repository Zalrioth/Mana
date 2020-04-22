#include "mana/graphics/entities/model.h"

int model_init(struct Model* model, struct VulkanRenderer* vulkan_renderer, char* mode_path, char* texture_path, int max_weights, struct Shader* shader) {
  struct XmlNode* node = xml_parser_load_xml_file(mode_path);
  struct SkinningData* skinning_data = skin_loader_extract_skin_data(xml_node_get_child(node, "library_controllers"), max_weights);

  model->joints = skeleton_loader_extract_bone_data(xml_node_get_child(node, "library_visual_scenes"), skinning_data->joint_order);
  model->model_mesh = geometry_loader_extract_model_data(xml_node_get_child(node, "library_geometries"), skinning_data->vertices_skin_data);
  model->model_texture = malloc(sizeof(struct Texture));
  texture_init(model->model_texture, vulkan_renderer, texture_path);

  // Vertex buffer
  VkDeviceSize vertex_buffer_size = model->model_mesh->vertices->memory_size * model->model_mesh->vertices->size;
  VkBuffer vertex_staging_buffer = {0};
  VkDeviceMemory vertex_staging_buffer_memory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, vertex_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vertex_staging_buffer, &vertex_staging_buffer_memory);

  void* vertex_data;
  vkMapMemory(vulkan_renderer->device, vertex_staging_buffer_memory, 0, vertex_buffer_size, 0, &vertex_data);
  memcpy(vertex_data, model->model_mesh->vertices->items, vertex_buffer_size);
  vkUnmapMemory(vulkan_renderer->device, vertex_staging_buffer_memory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, vertex_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &model->vertex_buffer, &model->vertex_buffer_memory);

  copy_buffer(vulkan_renderer, vertex_staging_buffer, model->vertex_buffer, vertex_buffer_size);

  vkDestroyBuffer(vulkan_renderer->device, vertex_staging_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, vertex_staging_buffer_memory, NULL);

  // Index buffer
  VkDeviceSize index_buffer_size = model->model_mesh->indices->memory_size * model->model_mesh->indices->size;
  VkBuffer index_staging_buffer = {0};
  VkDeviceMemory index_staging_buffer_memory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, index_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &index_staging_buffer, &index_staging_buffer_memory);

  void* index_data;
  vkMapMemory(vulkan_renderer->device, index_staging_buffer_memory, 0, index_buffer_size, 0, &index_data);
  memcpy(index_data, model->model_mesh->indices->items, index_buffer_size);
  vkUnmapMemory(vulkan_renderer->device, index_staging_buffer_memory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, index_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &model->index_buffer, &model->index_buffer_memory);

  copy_buffer(vulkan_renderer, index_staging_buffer, model->index_buffer, index_buffer_size);

  vkDestroyBuffer(vulkan_renderer->device, index_staging_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, index_staging_buffer_memory, NULL);

  // Uniform buffer
  VkDeviceSize uniform_buffer_size = sizeof(struct ModelUniformBufferObject);
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, uniform_buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &model->uniform_buffer, &model->uniform_buffers_memory);

  // Descriptor sets
  VkDescriptorSetLayout layout = {0};
  layout = shader->descriptor_set_layout;

  VkDescriptorSetAllocateInfo alloc_info = {0};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = shader->descriptor_pool;
  alloc_info.descriptorSetCount = 1;
  alloc_info.pSetLayouts = &layout;

  if (vkAllocateDescriptorSets(vulkan_renderer->device, &alloc_info, &model->descriptor_set) != VK_SUCCESS) {
    fprintf(stderr, "failed to allocate descriptor sets!\n");
    return 0;
  }

  VkDescriptorBufferInfo buffer_info = {0};
  buffer_info.buffer = model->uniform_buffer;
  buffer_info.offset = 0;
  buffer_info.range = sizeof(struct ModelUniformBufferObject);

  VkDescriptorImageInfo image_info = {0};
  image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  image_info.imageView = model->model_texture->texture_image_view;
  image_info.sampler = model->model_texture->texture_sampler;

  VkWriteDescriptorSet dcs[2];
  memset(dcs, 0, sizeof(dcs));

  dcs[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  dcs[0].dstSet = model->descriptor_set;
  dcs[0].dstBinding = 0;
  dcs[0].dstArrayElement = 0;
  dcs[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  dcs[0].descriptorCount = 1;
  dcs[0].pBufferInfo = &buffer_info;

  dcs[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  dcs[1].dstSet = model->descriptor_set;
  dcs[1].dstBinding = 1;
  dcs[1].dstArrayElement = 0;
  dcs[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  dcs[1].descriptorCount = 1;
  dcs[1].pImageInfo = &image_info;

  vkUpdateDescriptorSets(vulkan_renderer->device, 2, dcs, 0, NULL);

  return MODEL_SUCCESS;

  // If animated
  //XmlNode node = XmlParser.loadXmlFile(colladaFile);
  //XmlNode animNode = node.getChild("library_animations");
  //XmlNode jointsNode = node.getChild("library_visual_scenes");
  //AnimationLoader loader = new AnimationLoader(animNode, jointsNode);
  //AnimationData animData = loader.extractAnimation();
  //return animData;
}