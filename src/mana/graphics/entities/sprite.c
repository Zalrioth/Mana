#include "mana/graphics/entities/sprite.h"

void calc_normal(vec3 p1, vec3 p2, vec3 p3, vec3* dest) {
  vec3 v1;
  glm_vec3_sub(p2, p1, v1);

  vec3 v2;
  glm_vec3_sub(p3, p1, v2);

  (*dest)[0] = (v1[1] * v2[2]) - (v1[2] - v2[1]);
  (*dest)[1] = -((v2[2] * v1[0]) - (v2[0] * v1[2]));
  (*dest)[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
}

int sprite_init(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer, struct Shader* shader) {
  sprite->image_mesh = calloc(1, sizeof(struct Mesh));
  mesh_init(sprite->image_mesh);

  sprite->image_texture = calloc(1, sizeof(struct Texture));
  texture_init(sprite->image_texture, vulkan_renderer, "./Assets/textures/10bit.psd");

  vec3 pos1 = {-0.5f, -0.5f, 0.0f};
  vec3 pos2 = {0.5f, -0.5f, 0.0f};
  vec3 pos3 = {0.5f, 0.5f, 0.0f};
  vec3 pos4 = {-0.5f, 0.5f, 0.0f};

  vec3 norm1;
  calc_normal(pos1, pos2, pos3, &norm1);

  vec3 norm2;
  calc_normal(pos2, pos3, pos4, &norm2);

  mesh_assign_vertex(sprite->image_mesh->vertices, pos1[0], pos1[1], pos1[2], norm1[0], norm1[1], norm1[2], 1.0f, 0.0f);
  mesh_assign_vertex(sprite->image_mesh->vertices, pos2[0], pos2[1], pos2[2], norm1[0], norm1[1], norm1[2], 0.0f, 0.0f);
  mesh_assign_vertex(sprite->image_mesh->vertices, pos3[0], pos3[1], pos3[2], norm1[0], norm1[1], norm1[2], 0.0f, 1.0f);
  mesh_assign_vertex(sprite->image_mesh->vertices, pos4[0], pos4[1], pos4[2], norm2[0], norm2[1], norm2[2], 1.0f, 1.0f);

  mesh_assign_indice(sprite->image_mesh->indices, 0);
  mesh_assign_indice(sprite->image_mesh->indices, 1);
  mesh_assign_indice(sprite->image_mesh->indices, 2);
  mesh_assign_indice(sprite->image_mesh->indices, 2);
  mesh_assign_indice(sprite->image_mesh->indices, 3);
  mesh_assign_indice(sprite->image_mesh->indices, 0);

  //int error_code;

  sprite_create_vertex_buffer(sprite, vulkan_renderer);
  sprite_create_index_buffer(sprite, vulkan_renderer);
  sprite_create_uniform_buffers(sprite, vulkan_renderer);
  //sprite_create_descriptor_sets(sprite, vulkan_renderer);

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
  buffer_info.range = sizeof(struct UniformBufferObject);

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

  /*vulkan_index_buffer_error:
  vulkan_index_buffer_cleanup(vulkan_renderer);
vulkan_vertex_buffer_error:
  vulkan_vertex_buffer_cleanup(vulkan_renderer);
vulkan_texture_error:
  vulkan_texture_cleanup(vulkan_renderer);
  //
  for (int buffer_num = 0; buffer_num < MAX_SWAP_CHAIN_FRAMES; buffer_num++) {
    vkDestroyBuffer(vulkan_renderer->device, vulkan_renderer->uniform_buffers[buffer_num], NULL);
    vkFreeMemory(vulkan_renderer->device, vulkan_renderer->uniform_buffers_memory[buffer_num], NULL);
  }
  //
vulkan_desriptor_set_layout_error:
  vulkan_descriptor_set_layout_cleanup(vulkan_renderer);*/
}

/*int blit_sprite_init(struct BlitSprite* blit_sprite, struct VulkanRenderer* vulkan_renderer, struct Shader* shader) {
  sprite_init(&blit_sprite->sprite, vulkan_renderer);

  VkDescriptorSetLayout layouts[MAX_SWAP_CHAIN_FRAMES];
  memset(layouts, 0, sizeof(layouts));

  for (int loopNum = 0; loopNum < MAX_SWAP_CHAIN_FRAMES; loopNum++)
    layouts[loopNum] = shader->descriptor_set_layout;

  VkDescriptorSetAllocateInfo alloc_info = {0};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = shader->descriptor_pool;
  alloc_info.descriptorSetCount = MAX_SWAP_CHAIN_FRAMES;
  alloc_info.pSetLayouts = layouts;

  if (vkAllocateDescriptorSets(vulkan_renderer->device, &alloc_info, blit_sprite->sprite.descriptor_sets) != VK_SUCCESS) {
    fprintf(stderr, "failed to allocate descriptor sets!\n");
    return 0;
  }

  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
    VkDescriptorBufferInfo buffer_info = {0};
    buffer_info.buffer = blit_sprite->sprite.uniform_buffers[i];
    buffer_info.offset = 0;
    buffer_info.range = sizeof(struct UniformBufferObject);

    VkDescriptorImageInfo image_info = {0};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = blit_sprite->sprite.image_texture->texture_image_view;
    image_info.sampler = blit_sprite->sprite.image_texture->texture_sampler;

    VkWriteDescriptorSet dcs[2];
    memset(dcs, 0, sizeof(dcs));

    dcs[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    dcs[0].dstSet = blit_sprite->sprite.descriptor_sets[i];
    dcs[0].dstBinding = 0;
    dcs[0].dstArrayElement = 0;
    dcs[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    dcs[0].descriptorCount = 1;
    dcs[0].pBufferInfo = &buffer_info;

    dcs[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    dcs[1].dstSet = blit_sprite->sprite.descriptor_sets[i];
    dcs[1].dstBinding = 1;
    dcs[1].dstArrayElement = 0;
    dcs[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    dcs[1].descriptorCount = 1;
    dcs[1].pImageInfo = &image_info;

    vkUpdateDescriptorSets(vulkan_renderer->device, 2, dcs, 0, NULL);
  }

  return 1;
}*/

void sprite_delete(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  vulkan_index_buffer_cleanup(sprite, vulkan_renderer);
  vulkan_vertex_buffer_cleanup(sprite, vulkan_renderer);
  vkDestroyBuffer(vulkan_renderer->device, sprite->uniform_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, sprite->uniform_buffers_memory, NULL);
  mesh_delete(sprite->image_mesh);
  free(sprite->image_mesh);
  vulkan_texture_cleanup(sprite, vulkan_renderer);
}

int sprite_create_vertex_buffer(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  VkDeviceSize buffer_size = sprite->image_mesh->vertices->memory_size * sprite->image_mesh->vertices->size;
  //VkDeviceSize bufferSize = sizeof(vulkan_renderer->imageVertices.items[0]) * vulkan_renderer->imageVertices.total;

  VkBuffer staging_buffer = {0};
  VkDeviceMemory stagingBufferMemory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer, &stagingBufferMemory);

  void* data;
  vkMapMemory(vulkan_renderer->device, stagingBufferMemory, 0, buffer_size, 0, &data);
  memcpy(data, sprite->image_mesh->vertices->items, buffer_size);
  vkUnmapMemory(vulkan_renderer->device, stagingBufferMemory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &sprite->vertex_buffer, &sprite->vertex_buffer_memory);

  copy_buffer(vulkan_renderer, staging_buffer, sprite->vertex_buffer, buffer_size);

  vkDestroyBuffer(vulkan_renderer->device, staging_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, stagingBufferMemory, NULL);

  return 0;
}

int sprite_create_index_buffer(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  VkDeviceSize buffer_size = sprite->image_mesh->indices->memory_size * sprite->image_mesh->indices->size;
  //VkDeviceSize bufferSize = sizeof(vulkan_renderer->imageIndices.items[0]) * vulkan_renderer->imageIndices.total;

  VkBuffer staging_buffer = {0};
  VkDeviceMemory staging_buffer_memory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer, &staging_buffer_memory);

  void* data;
  vkMapMemory(vulkan_renderer->device, staging_buffer_memory, 0, buffer_size, 0, &data);
  memcpy(data, sprite->image_mesh->indices->items, buffer_size);
  vkUnmapMemory(vulkan_renderer->device, staging_buffer_memory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &sprite->index_buffer, &sprite->index_buffer_memory);

  copy_buffer(vulkan_renderer, staging_buffer, sprite->index_buffer, buffer_size);

  vkDestroyBuffer(vulkan_renderer->device, staging_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, staging_buffer_memory, NULL);

  return 0;
}

int sprite_create_uniform_buffers(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  VkDeviceSize buffer_size = sizeof(struct UniformBufferObject);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &sprite->uniform_buffer, &sprite->uniform_buffers_memory);

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////
void vulkan_index_buffer_cleanup(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  vkDestroyBuffer(vulkan_renderer->device, sprite->index_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, sprite->index_buffer_memory, NULL);
}

void vulkan_vertex_buffer_cleanup(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  vkDestroyBuffer(vulkan_renderer->device, sprite->vertex_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, sprite->vertex_buffer_memory, NULL);
}

void vulkan_texture_cleanup(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  texture_delete(vulkan_renderer, sprite->image_texture);
  free(sprite->image_texture);
}
