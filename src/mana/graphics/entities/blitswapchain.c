#include "mana/graphics/entities/blitswapchain.h"

int blit_swapchain_init(struct BlitSwapchain* blit_swapchain, struct VulkanRenderer* vulkan_renderer, struct Shader* shader) {
  VkDescriptorSetLayout layout = {0};
  layout = shader->descriptor_set_layout;

  VkDescriptorSetAllocateInfo alloc_info = {0};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = shader->descriptor_pool;
  alloc_info.descriptorSetCount = 1;
  alloc_info.pSetLayouts = &layout;

  if (vkAllocateDescriptorSets(vulkan_renderer->device, &alloc_info, &blit_swapchain->descriptor_set) != VK_SUCCESS) {
    fprintf(stderr, "failed to allocate descriptor sets!\n");
    return 0;
  }

  VkDescriptorImageInfo image_info = {0};
  image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  image_info.imageView = vulkan_renderer->gbuffer->color_image_view;
  image_info.sampler = vulkan_renderer->gbuffer->texture_sampler;

  VkWriteDescriptorSet dc;
  memset(&dc, 0, sizeof(dc));

  dc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  dc.dstSet = blit_swapchain->descriptor_set;
  dc.dstBinding = 0;
  dc.dstArrayElement = 0;
  dc.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  dc.descriptorCount = 1;
  dc.pImageInfo = &image_info;

  vkUpdateDescriptorSets(vulkan_renderer->device, 1, &dc, 0, NULL);

  // Stuff
  blit_swapchain->image_mesh = calloc(1, sizeof(struct Mesh));
  mesh_init(blit_swapchain->image_mesh);

  mesh_assign_vertex(blit_swapchain->image_mesh->vertices, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  mesh_assign_vertex(blit_swapchain->image_mesh->vertices, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
  mesh_assign_vertex(blit_swapchain->image_mesh->vertices, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
  mesh_assign_vertex(blit_swapchain->image_mesh->vertices, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
  mesh_assign_indice(blit_swapchain->image_mesh->indices, 0);
  mesh_assign_indice(blit_swapchain->image_mesh->indices, 1);
  mesh_assign_indice(blit_swapchain->image_mesh->indices, 2);
  mesh_assign_indice(blit_swapchain->image_mesh->indices, 2);
  mesh_assign_indice(blit_swapchain->image_mesh->indices, 3);
  mesh_assign_indice(blit_swapchain->image_mesh->indices, 0);

  // Vertex
  VkDeviceSize buffer_size_vertex = blit_swapchain->image_mesh->vertices->memory_size * blit_swapchain->image_mesh->vertices->size;
  //VkDeviceSize bufferSize = sizeof(vulkan_renderer->imageVertices.items[0]) * vulkan_renderer->imageVertices.total;

  VkBuffer staging_buffer_vertex = {0};
  VkDeviceMemory stagingBufferMemory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size_vertex, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer_vertex, &stagingBufferMemory);

  void* data_vertex;
  vkMapMemory(vulkan_renderer->device, stagingBufferMemory, 0, buffer_size_vertex, 0, &data_vertex);
  memcpy(data_vertex, blit_swapchain->image_mesh->vertices->items, buffer_size_vertex);
  vkUnmapMemory(vulkan_renderer->device, stagingBufferMemory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size_vertex, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &blit_swapchain->vertex_buffer, &blit_swapchain->vertex_buffer_memory);

  copy_buffer(vulkan_renderer, staging_buffer_vertex, blit_swapchain->vertex_buffer, buffer_size_vertex);

  vkDestroyBuffer(vulkan_renderer->device, staging_buffer_vertex, NULL);
  vkFreeMemory(vulkan_renderer->device, stagingBufferMemory, NULL);

  // Index
  VkDeviceSize buffer_size_index = blit_swapchain->image_mesh->indices->memory_size * blit_swapchain->image_mesh->indices->size;
  //VkDeviceSize bufferSize = sizeof(vulkan_renderer->imageIndices.items[0]) * vulkan_renderer->imageIndices.total;

  VkBuffer staging_buffer_index = {0};
  VkDeviceMemory staging_buffer_memory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size_index, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer_index, &staging_buffer_memory);

  void* data_index;
  vkMapMemory(vulkan_renderer->device, staging_buffer_memory, 0, buffer_size_index, 0, &data_index);
  memcpy(data_index, blit_swapchain->image_mesh->indices->items, buffer_size_index);
  vkUnmapMemory(vulkan_renderer->device, staging_buffer_memory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size_index, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &blit_swapchain->index_buffer, &blit_swapchain->index_buffer_memory);

  copy_buffer(vulkan_renderer, staging_buffer_index, blit_swapchain->index_buffer, buffer_size_index);

  vkDestroyBuffer(vulkan_renderer->device, staging_buffer_index, NULL);
  vkFreeMemory(vulkan_renderer->device, staging_buffer_memory, NULL);

  return 1;
}

void blit_swapchain_delete(struct BlitSwapchain* blit_swapchain, struct VulkanRenderer* vulkan_renderer) {
  //vulkan_index_buffer_cleanup(blit_swapchain, vulkan_renderer);
  //vulkan_vertex_buffer_cleanup(blit_swapchain, vulkan_renderer);
  mesh_delete(blit_swapchain->image_mesh);
  free(blit_swapchain->image_mesh);
}