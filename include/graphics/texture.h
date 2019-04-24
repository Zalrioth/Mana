#pragma once
#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "core/common.h"
#include "core/window.h"

struct Texture {
    unsigned int id;
    char* type;
    char* path;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
};

//https://stackoverflow.com/questions/9999512/forward-declaration-of-a-struct-in-c
struct Window;

void texture_init();
void texture_delete();
int createTextureImage(struct Window* window, struct Texture* texture);
int createTextureSampler(struct Window* window, struct Texture* texture);
int createTextureImageView(struct Window* window, struct Texture* texture);
VkImageView createImageView(struct Window* window, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
int createImage(struct Window* window, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory);
VkFormat findDepthFormat(struct Window* window);
int transitionImageLayout(struct Window* window, VkImage* image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
VkCommandBuffer beginSingleTimeCommands();
void endSingleTimeCommands(struct Window* window, VkCommandBuffer commandBuffer);
int createBuffer(struct Window* window, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
void copyBufferToImage(struct Window* window, VkBuffer* buffer, VkImage* image, uint32_t width, uint32_t height);
uint32_t findMemoryType(struct Window* window, uint32_t typeFilter, VkMemoryPropertyFlags properties);

#endif // TEXTURE_H_
