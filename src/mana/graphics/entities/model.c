#include "mana/graphics/entities/model.h"

int model_init(struct Model* model, struct GPUAPI* gpu_api, char* node_path, char* texture_path, int max_weights, struct Shader* shader, enum FilterType filter_type) {
  VkFilter filter = (filter_type == FILTER_NEAREST) ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;
  struct XmlNode* collada_node = xml_parser_load_xml_file(node_path);
  struct XmlNode* library_controllers_node = xml_node_get_child(collada_node, "library_controllers");  // If texture is null, use custom 8x8 ubo color palette
  bool animated = !(library_controllers_node == NULL || library_controllers_node->child_nodes == NULL || library_controllers_node->child_nodes->num_buckets == 0);
  size_t ubo_buffer_size = animated ? sizeof(struct ModelUniformBufferObject) : sizeof(struct ModelStaticUniformBufferObject);

  struct ModelCache* model_cache = malloc(sizeof(struct ModelCache));
  struct SkinningData* skinning_data = NULL;
  if (animated) {
    skinning_data = skin_loader_extract_skin_data(library_controllers_node, max_weights);
    model_cache->joints = skeleton_loader_extract_bone_data(xml_node_get_child(collada_node, "library_visual_scenes"), skinning_data->joint_order);
    model_cache->model_mesh = geometry_loader_extract_model_data(xml_node_get_child(collada_node, "library_geometries"), skinning_data->vertices_skin_data, animated);

    model->root_joint = model_create_joints(model_cache->joints->head_joint);
    model->animator = malloc(sizeof(struct Animator));
    animator_init(model->animator, model);
    joint_calc_inverse_bind_transform(model->root_joint, GLM_MAT4_IDENTITY);

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
  } else
    model_cache->model_mesh = geometry_loader_extract_model_data(xml_node_get_child(collada_node, "library_geometries"), NULL, animated);

  model->model_raw = model_cache;

  model_cache->model_texture = malloc(sizeof(struct Texture));
  texture_init(model_cache->model_texture, gpu_api->vulkan_state, texture_path, filter);

  graphics_utils_setup_vertex_buffer(gpu_api->vulkan_state, model->model_raw->model_mesh->vertices, &model->vertex_buffer, &model->vertex_buffer_memory);
  graphics_utils_setup_index_buffer(gpu_api->vulkan_state, model->model_raw->model_mesh->indices, &model->index_buffer, &model->index_buffer_memory);
  graphics_utils_setup_uniform_buffer(gpu_api->vulkan_state, ubo_buffer_size, &model->uniform_buffer, &model->uniform_buffers_memory);
  graphics_utils_setup_uniform_buffer(gpu_api->vulkan_state, sizeof(struct LightingUniformBufferObject), &model->lighting_uniform_buffer, &model->lighting_uniform_buffers_memory);
  graphics_utils_setup_descriptor(gpu_api->vulkan_state, shader->descriptor_set_layout, shader->descriptor_pool, &model->descriptor_set);

  VkWriteDescriptorSet dcs[3] = {0};

  graphics_utils_setup_descriptor_buffer(gpu_api->vulkan_state, dcs, 0, &model->descriptor_set, (VkDescriptorBufferInfo[]){graphics_utils_setup_descriptor_buffer_info(ubo_buffer_size, &model->uniform_buffer)});
  graphics_utils_setup_descriptor_buffer(gpu_api->vulkan_state, dcs, 1, &model->descriptor_set, (VkDescriptorBufferInfo[]){graphics_utils_setup_descriptor_buffer_info(sizeof(struct LightingUniformBufferObject), &model->lighting_uniform_buffer)});
  graphics_utils_setup_descriptor_image(gpu_api->vulkan_state, dcs, 2, &model->descriptor_set, (VkDescriptorImageInfo[]){graphics_utils_setup_descriptor_image_info(&model_cache->model_texture->texture_image_view, &model_cache->model_texture->texture_sampler)});

  vkUpdateDescriptorSets(gpu_api->vulkan_state->device, 3, dcs, 0, NULL);

  xml_parser_delete(collada_node);

  return MODEL_SUCCESS;
}

void model_delete(struct GPUAPI* gpu_api, struct Model* model) {
  vkDestroyBuffer(gpu_api->vulkan_state->device, model->index_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, model->index_buffer_memory, NULL);

  vkDestroyBuffer(gpu_api->vulkan_state->device, model->vertex_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, model->vertex_buffer_memory, NULL);

  vkDestroyBuffer(gpu_api->vulkan_state->device, model->uniform_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, model->uniform_buffers_memory, NULL);

  vkDestroyBuffer(gpu_api->vulkan_state->device, model->lighting_uniform_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, model->lighting_uniform_buffers_memory, NULL);

  // TODO: Delete uniform colors if needed
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
  //glm_mat4_quat(mat, rotation);
  mat4_to_collada_quaternion(mat, rotation);

  struct JointTransform* joint_transform = malloc(sizeof(struct JointTransform));
  joint_transform_init(joint_transform, translation, rotation);
  return joint_transform;
}

void model_get_joint_transforms(struct Joint* head_joint, mat4 dest[MAX_JOINTS]) {
  glm_mat4_copy(head_joint->animation_transform, dest[head_joint->index]);
  for (int child_joint_num = 0; child_joint_num < array_list_size(head_joint->children); child_joint_num++)
    model_get_joint_transforms((struct Joint*)array_list_get(head_joint->children, child_joint_num), dest);
}
