#include "mana/graphics/entities/model.h"

int model_init(struct Model* model, struct VulkanRenderer* vulkan_renderer, char* node_path, char* texture_path, int max_weights, struct Shader* shader) {
  struct XmlNode* collada_node = xml_parser_load_xml_file(node_path);
  struct SkinningData* skinning_data = skin_loader_extract_skin_data(xml_node_get_child(collada_node, "library_controllers"), max_weights);

  struct ModelCache* model_cache = malloc(sizeof(struct ModelCache));
  model_cache->joints = skeleton_loader_extract_bone_data(xml_node_get_child(collada_node, "library_visual_scenes"), skinning_data->joint_order);
  model_cache->model_mesh = geometry_loader_extract_model_data(xml_node_get_child(collada_node, "library_geometries"), skinning_data->vertices_skin_data);
  model_cache->model_texture = malloc(sizeof(struct Texture));
  // TODO: This should point to texture in texture cache
  texture_init(model_cache->model_texture, vulkan_renderer, texture_path);

  model->model_raw = model_cache;
  model->root_joint = model_create_joints(model_cache->joints->head_joint);
  model->animator = malloc(sizeof(struct Animator));
  animator_init(model->animator, model);
  joint_calc_inverse_bind_transform(model->root_joint, GLM_MAT4_IDENTITY);

  // Vertex buffer
  VkDeviceSize vertex_buffer_size = model_cache->model_mesh->vertices->memory_size * model_cache->model_mesh->vertices->size;
  VkBuffer vertex_staging_buffer = {0};
  VkDeviceMemory vertex_staging_buffer_memory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, vertex_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vertex_staging_buffer, &vertex_staging_buffer_memory);

  void* vertex_data;
  vkMapMemory(vulkan_renderer->device, vertex_staging_buffer_memory, 0, vertex_buffer_size, 0, &vertex_data);
  memcpy(vertex_data, model_cache->model_mesh->vertices->items, vertex_buffer_size);
  vkUnmapMemory(vulkan_renderer->device, vertex_staging_buffer_memory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, vertex_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &model->vertex_buffer, &model->vertex_buffer_memory);

  copy_buffer(vulkan_renderer, vertex_staging_buffer, model->vertex_buffer, vertex_buffer_size);

  vkDestroyBuffer(vulkan_renderer->device, vertex_staging_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, vertex_staging_buffer_memory, NULL);

  // Index buffer
  VkDeviceSize index_buffer_size = model_cache->model_mesh->indices->memory_size * model_cache->model_mesh->indices->size;
  VkBuffer index_staging_buffer = {0};
  VkDeviceMemory index_staging_buffer_memory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, index_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &index_staging_buffer, &index_staging_buffer_memory);

  void* index_data;
  vkMapMemory(vulkan_renderer->device, index_staging_buffer_memory, 0, index_buffer_size, 0, &index_data);
  memcpy(index_data, model_cache->model_mesh->indices->items, index_buffer_size);
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
  image_info.imageView = model_cache->model_texture->texture_image_view;
  image_info.sampler = model_cache->model_texture->texture_sampler;

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

  // If animated
  struct XmlNode* anim_node = xml_node_get_child(collada_node, "library_animations");
  struct XmlNode* joints_node = xml_node_get_child(collada_node, "library_visual_scenes");
  struct AnimationData* animation_data = animation_extract_animation(anim_node, joints_node);

  struct ArrayList* frames = malloc(sizeof(struct ArrayList));
  array_list_init(frames);

  for (int frame_num = 0; frame_num < array_list_size(animation_data->key_frames); frame_num++)
    array_list_add(frames, model_create_key_frame(array_list_get(animation_data->key_frames, frame_num)));

  model->animation = malloc(sizeof(struct Animation));
  animation_init(model->animation, animation_data->length_seconds, frames);

  animator_do_animation(model->animator, model->animation);

  return MODEL_SUCCESS;
}

struct Joint* model_create_joints(struct JointData* root_joint_data) {
  struct Joint* joint = malloc(sizeof(struct Joint));
  joint_init(joint, root_joint_data->index, root_joint_data->name_id, root_joint_data->bind_local_transform);

  for (int joint_child_num = 0; joint_child_num < array_list_size(root_joint_data->children); joint_child_num++) {
    struct JointData* child_joint = (struct JointData*)array_list_get(root_joint_data->children, joint_child_num);
    array_list_add(joint->children, model_create_joints(child_joint));
  }
  return joint;
}

struct KeyFrame* model_create_key_frame(struct KeyFrameData* data) {
  struct Map* map = malloc(sizeof(struct Map));
  map_init(map, sizeof(struct JointTransform));
  for (int joint_num = 0; joint_num < array_list_size(data->joint_transforms); joint_num++) {
    struct JointTransformData* joint_data = (struct JointTransformData*)array_list_get(data->joint_transforms, joint_num);
    struct JointTransform* joint_transform = model_create_transform(joint_data);
    map_set(map, joint_data->joint_name_id, joint_transform);
  }
  struct KeyFrame* key_frame = malloc(sizeof(struct KeyFrame));
  key_frame_init(key_frame, data->time, map);
  return key_frame;
}

struct JointTransform* model_create_transform(struct JointTransformData* data) {
  mat4 mat = GLM_MAT4_ZERO_INIT;
  glm_mat4_copy(data->joint_local_transform, mat);
  vec3 translation = {mat[3][0], mat[3][1], mat[3][2]};
  versor rotation = GLM_QUAT_IDENTITY_INIT;
  glm_mat4_quat(mat, rotation);
  struct JointTransform* joint_transform = malloc(sizeof(struct JointTransform));
  joint_transform_init(joint_transform, translation, rotation);
  return joint_transform;
}

void model_get_joint_transforms(struct Joint* head_joint, mat4 dest[MAX_JOINTS]) {
  glm_mat4_copy(head_joint->animation_transform, dest[head_joint->index]);
  for (int child_joint_num = 0; child_joint_num < array_list_size(head_joint->children); child_joint_num++)
    model_get_joint_transforms((struct Joint*)array_list_get(head_joint->children, child_joint_num), dest);
}
