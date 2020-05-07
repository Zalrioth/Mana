#include "mana/graphics/entities/sprite.h"

//void calc_normal(vec3 p1, vec3 p2, vec3 p3, float* dest) {
//  vec3 v1;
//  glm_vec3_sub(p2, p1, v1);
//
//  vec3 v2;
//  glm_vec3_sub(p3, p1, v2);
//
//  glm_vec3_crossn(v1, v2, dest);
//}

int sprite_init(struct Sprite* sprite, struct VulkanState* vulkan_renderer, struct Shader* shader) {
  sprite->image_mesh = calloc(1, sizeof(struct Mesh));
  mesh_sprite_init(sprite->image_mesh);

  sprite->image_texture = calloc(1, sizeof(struct Texture));
  texture_init(sprite->image_texture, vulkan_renderer, "./assets/textures/alpha.png");

  vec3 pos1 = {-0.5f, -0.5f, 0.0f};
  vec3 pos2 = {0.5f, -0.5f, 0.0f};
  vec3 pos3 = {0.5f, 0.5f, 0.0f};
  vec3 pos4 = {-0.5f, 0.5f, 0.0f};

  vec2 uv1 = {1.0f, 0.0f};
  vec2 uv2 = {0.0f, 0.0f};
  vec2 uv3 = {0.0f, 1.0f};
  vec2 uv4 = {1.0f, 1.0f};

  mesh_sprite_assign_vertex(sprite->image_mesh->vertices, pos1[0], pos1[1], pos1[2], uv1[0], uv1[1]);
  mesh_sprite_assign_vertex(sprite->image_mesh->vertices, pos2[0], pos2[1], pos2[2], uv2[0], uv2[1]);
  mesh_sprite_assign_vertex(sprite->image_mesh->vertices, pos3[0], pos3[1], pos3[2], uv3[0], uv3[1]);
  mesh_sprite_assign_vertex(sprite->image_mesh->vertices, pos4[0], pos4[1], pos4[2], uv4[0], uv4[1]);

  mesh_assign_indice(sprite->image_mesh->indices, 0);
  mesh_assign_indice(sprite->image_mesh->indices, 1);
  mesh_assign_indice(sprite->image_mesh->indices, 2);
  mesh_assign_indice(sprite->image_mesh->indices, 2);
  mesh_assign_indice(sprite->image_mesh->indices, 3);
  mesh_assign_indice(sprite->image_mesh->indices, 0);

  // Vertex buffer
  VkDeviceSize vertex_buffer_size = sprite->image_mesh->vertices->memory_size * sprite->image_mesh->vertices->size;
  VkBuffer vertex_staging_buffer = {0};
  VkDeviceMemory vertex_staging_buffer_memory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, vertex_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vertex_staging_buffer, &vertex_staging_buffer_memory);

  void* vertex_data;
  vkMapMemory(vulkan_renderer->device, vertex_staging_buffer_memory, 0, vertex_buffer_size, 0, &vertex_data);
  memcpy(vertex_data, sprite->image_mesh->vertices->items, vertex_buffer_size);
  vkUnmapMemory(vulkan_renderer->device, vertex_staging_buffer_memory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, vertex_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &sprite->vertex_buffer, &sprite->vertex_buffer_memory);

  copy_buffer(vulkan_renderer, vertex_staging_buffer, sprite->vertex_buffer, vertex_buffer_size);

  vkDestroyBuffer(vulkan_renderer->device, vertex_staging_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, vertex_staging_buffer_memory, NULL);

  // Index buffer
  VkDeviceSize index_buffer_size = sprite->image_mesh->indices->memory_size * sprite->image_mesh->indices->size;
  VkBuffer index_staging_buffer = {0};
  VkDeviceMemory index_staging_buffer_memory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, index_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &index_staging_buffer, &index_staging_buffer_memory);

  void* index_data;
  vkMapMemory(vulkan_renderer->device, index_staging_buffer_memory, 0, index_buffer_size, 0, &index_data);
  memcpy(index_data, sprite->image_mesh->indices->items, index_buffer_size);
  vkUnmapMemory(vulkan_renderer->device, index_staging_buffer_memory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, index_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &sprite->index_buffer, &sprite->index_buffer_memory);

  copy_buffer(vulkan_renderer, index_staging_buffer, sprite->index_buffer, index_buffer_size);

  vkDestroyBuffer(vulkan_renderer->device, index_staging_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, index_staging_buffer_memory, NULL);

  // Uniform buffer
  VkDeviceSize uniform_buffer_size = sizeof(struct SpriteUniformBufferObject);
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, uniform_buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &sprite->uniform_buffer, &sprite->uniform_buffers_memory);

  // Descriptor sets
  VkDescriptorSetLayout layout = {0};
  layout = shader->descriptor_set_layout;

  VkDescriptorSetAllocateInfo alloc_info = {0};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = shader->descriptor_pool;
  alloc_info.descriptorSetCount = 1;
  alloc_info.pSetLayouts = &layout;

  if (vkAllocateDescriptorSets(vulkan_renderer->device, &alloc_info, &sprite->descriptor_set) != VK_SUCCESS) {
    fprintf(stderr, "failed to allocate descriptor sets!\n");
    return 0;
  }

  VkDescriptorBufferInfo buffer_info = {0};
  buffer_info.buffer = sprite->uniform_buffer;
  buffer_info.offset = 0;
  buffer_info.range = sizeof(struct SpriteUniformBufferObject);

  VkDescriptorImageInfo image_info = {0};
  image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  image_info.imageView = sprite->image_texture->texture_image_view;
  image_info.sampler = sprite->image_texture->texture_sampler;

  VkWriteDescriptorSet dcs[2];
  memset(dcs, 0, sizeof(dcs));

  dcs[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  dcs[0].dstSet = sprite->descriptor_set;
  dcs[0].dstBinding = 0;
  dcs[0].dstArrayElement = 0;
  dcs[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  dcs[0].descriptorCount = 1;
  dcs[0].pBufferInfo = &buffer_info;

  dcs[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  dcs[1].dstSet = sprite->descriptor_set;
  dcs[1].dstBinding = 1;
  dcs[1].dstArrayElement = 0;
  dcs[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  dcs[1].descriptorCount = 1;
  dcs[1].pImageInfo = &image_info;

  vkUpdateDescriptorSets(vulkan_renderer->device, 2, dcs, 0, NULL);

  return SPRITE_SUCCESS;
}

void sprite_delete(struct Sprite* sprite, struct VulkanState* vulkan_renderer) {
  vkDestroyBuffer(vulkan_renderer->device, sprite->index_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, sprite->index_buffer_memory, NULL);

  vkDestroyBuffer(vulkan_renderer->device, sprite->vertex_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, sprite->vertex_buffer_memory, NULL);

  vkDestroyBuffer(vulkan_renderer->device, sprite->uniform_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, sprite->uniform_buffers_memory, NULL);

  mesh_delete(sprite->image_mesh);
  free(sprite->image_mesh);

  texture_delete(vulkan_renderer, sprite->image_texture);
  free(sprite->image_texture);
}
