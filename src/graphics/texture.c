#include "graphics/texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <graphics/stb_image.h>

void texture_init(struct Texture *texture, char *path) {
  // Note: Extra 0 needed to ensure end of string
  int pathLength = strlen(path);
  texture->path = malloc(pathLength + 1);
  memset(texture->path, 0, pathLength);
  strcpy(texture->path, path);

  // Todo: Extract filetype
  int typeLength = 4;
  texture->type = malloc(typeLength + 1);
  memset(texture->type, 0, typeLength + 1);
  strcpy(texture->type, ".tst");
}

void texture_delete(struct Window *window, struct Texture *texture) {
  vkDestroySampler(window->device, texture->textureSampler, NULL);
  vkDestroyImageView(window->device, texture->textureImageView, NULL);

  vkDestroyImage(window->device, texture->textureImage, NULL);
  vkFreeMemory(window->device, texture->textureImageMemory, NULL);

  free(texture->path);
  free(texture->type);
}

int texture_create_image(struct Window *window, struct Texture *texture) {
  int texWidth, texHeight, texChannels;
  stbi_uc *pixels = stbi_load(texture->path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
  VkDeviceSize imageSize = texWidth * texHeight * 4;

  if (!pixels) return -1;
  // printf("failed to load texture image!\n");

  VkBuffer stagingBuffer = {0};
  VkDeviceMemory stagingBufferMemory = {0};
  graphics_utils_create_buffer(window, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

  void *data;
  vkMapMemory(window->device, stagingBufferMemory, 0, imageSize, 0, &data);
  memcpy(data, pixels, imageSize);
  vkUnmapMemory(window->device, stagingBufferMemory);

  stbi_image_free(pixels);

  graphics_utils_create_image(window, texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texture->textureImage, &texture->textureImageMemory);

  graphics_utils_transition_image_layout(window, &texture->textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  texture_copy_buffer_to_image(window, &stagingBuffer, &texture->textureImage, texWidth, texHeight);
  graphics_utils_transition_image_layout(window, &texture->textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer(window->device, stagingBuffer, NULL);
  vkFreeMemory(window->device, stagingBufferMemory, NULL);

  return 0;
}

int texture_create_sampler(struct Window *window, struct Texture *texture) {
  VkSamplerCreateInfo samplerInfo = {0};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = 16;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  if (vkCreateSampler(window->device, &samplerInfo, NULL, &texture->textureSampler) != VK_SUCCESS) {
    return -1;
    printf("failed to create texture sampler!\n");
  }

  return 0;
}

int texture_create_texture_image_view(struct Window *window, struct Texture *texture) {
  texture->textureImageView = graphics_utils_create_image_view(window, texture->textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

  return 0;
}

void texture_copy_buffer_to_image(struct Window *window, VkBuffer *buffer, VkImage *image, uint32_t width, uint32_t height) {
  VkCommandBuffer commandBuffer = begin_single_time_commands(window);

  VkBufferImageCopy region = {0};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset.x = 0;
  region.imageOffset.y = 0;
  region.imageOffset.z = 0;
  region.imageExtent.width = width;
  region.imageExtent.height = height;
  region.imageExtent.depth = 1;

  vkCmdCopyBufferToImage(commandBuffer, *buffer, *image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  end_single_time_commands(window, commandBuffer);
}
