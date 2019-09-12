#include "graphics/sprite.h"

int sprite_init(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  sprite->image_mesh = calloc(1, sizeof(struct Mesh));
  mesh_init(sprite->image_mesh);

  sprite->image_texture = calloc(1, sizeof(struct Texture));
  texture_init(sprite->image_texture, "./Assets/textures/texture.jpg");

  mesh_init(sprite->image_mesh);

  mesh_assign_vertex(sprite->image_mesh->vertices, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  mesh_assign_vertex(sprite->image_mesh->vertices, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
  mesh_assign_vertex(sprite->image_mesh->vertices, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
  mesh_assign_vertex(sprite->image_mesh->vertices, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

  mesh_assign_vertex(sprite->image_mesh->vertices, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  mesh_assign_vertex(sprite->image_mesh->vertices, 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
  mesh_assign_vertex(sprite->image_mesh->vertices, 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
  mesh_assign_vertex(sprite->image_mesh->vertices, -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

  mesh_assign_indice(sprite->image_mesh->indices, 0);
  mesh_assign_indice(sprite->image_mesh->indices, 1);
  mesh_assign_indice(sprite->image_mesh->indices, 2);
  mesh_assign_indice(sprite->image_mesh->indices, 2);
  mesh_assign_indice(sprite->image_mesh->indices, 3);
  mesh_assign_indice(sprite->image_mesh->indices, 0);

  mesh_assign_indice(sprite->image_mesh->indices, 4);
  mesh_assign_indice(sprite->image_mesh->indices, 5);
  mesh_assign_indice(sprite->image_mesh->indices, 6);
  mesh_assign_indice(sprite->image_mesh->indices, 6);
  mesh_assign_indice(sprite->image_mesh->indices, 7);
  mesh_assign_indice(sprite->image_mesh->indices, 4);

  //int error_code;

  texture_create_image(vulkan_renderer, sprite->image_texture);
  texture_create_texture_image_view(vulkan_renderer, sprite->image_texture);
  texture_create_sampler(vulkan_renderer, sprite->image_texture);

  sprite_create_vertex_buffer(sprite, vulkan_renderer);
  sprite_create_index_buffer(sprite, vulkan_renderer);
  sprite_create_uniform_buffers(sprite, vulkan_renderer);
  create_descriptor_sets(sprite, vulkan_renderer);

  return NO_ERROR;

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

void sprite_delete(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  vulkan_index_buffer_cleanup(sprite, vulkan_renderer);
  vulkan_vertex_buffer_cleanup(sprite, vulkan_renderer);
  for (int buffer_num = 0; buffer_num < MAX_SWAP_CHAIN_FRAMES; buffer_num++) {
    vkDestroyBuffer(vulkan_renderer->device, sprite->uniform_buffers[buffer_num], NULL);
    vkFreeMemory(vulkan_renderer->device, sprite->uniform_buffers_memory[buffer_num], NULL);
  }
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

  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++)
    graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &sprite->uniform_buffers[i], &sprite->uniform_buffers_memory[i]);

  return 0;
}

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

int create_descriptor_sets(struct Sprite* sprite, struct VulkanRenderer* vulkan_renderer) {
  VkDescriptorSetLayout layouts[MAX_SWAP_CHAIN_FRAMES];
  memset(layouts, 0, sizeof(layouts));

  for (int loopNum = 0; loopNum < MAX_SWAP_CHAIN_FRAMES; loopNum++)
    layouts[loopNum] = vulkan_renderer->descriptor_set_layout;

  VkDescriptorSetAllocateInfo alloc_info = {0};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = vulkan_renderer->descriptor_pool;
  alloc_info.descriptorSetCount = MAX_SWAP_CHAIN_FRAMES;
  alloc_info.pSetLayouts = layouts;

  if (vkAllocateDescriptorSets(vulkan_renderer->device, &alloc_info, sprite->descriptor_sets) != VK_SUCCESS) {
    fprintf(stderr, "failed to allocate descriptor sets!\n");
    return -1;
  }

  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
    VkDescriptorBufferInfo buffer_info = {0};
    buffer_info.buffer = sprite->uniform_buffers[i];
    buffer_info.offset = 0;
    buffer_info.range = sizeof(struct UniformBufferObject);

    VkDescriptorImageInfo image_info = {0};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = sprite->image_texture->textureImageView;
    image_info.sampler = sprite->image_texture->textureSampler;

    //int descriptorSize = 2;
    struct Vector descriptor_writes;
    vector_init(&descriptor_writes, sizeof(VkWriteDescriptorSet));
    //VkWriteDescriptorSet descriptorWrites[descriptorSize];
    //memset(descriptorWrites, 0, sizeof(descriptorWrites));

    VkWriteDescriptorSet dcs1 = {0};
    dcs1.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    dcs1.dstSet = sprite->descriptor_sets[i];
    dcs1.dstBinding = descriptor_writes.size;
    dcs1.dstArrayElement = 0;
    dcs1.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    dcs1.descriptorCount = 1;
    dcs1.pBufferInfo = &buffer_info;

    vector_push_back(&descriptor_writes, &dcs1);

    VkWriteDescriptorSet dcs2 = {0};
    dcs2.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    dcs2.dstSet = sprite->descriptor_sets[i];
    dcs2.dstBinding = descriptor_writes.size;
    dcs2.dstArrayElement = 0;
    dcs2.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    dcs2.descriptorCount = 1;
    dcs2.pImageInfo = &image_info;

    vector_push_back(&descriptor_writes, &dcs2);

    vkUpdateDescriptorSets(vulkan_renderer->device, descriptor_writes.size, descriptor_writes.items, 0, NULL);

    vector_delete(&descriptor_writes);
  }

  return 0;
}
