#pragma once
#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "core/common.h"
#include "core/window.h"

struct Texture
{
    // TODO: Will probably need to use this later, maybe global static int that increments after each texture creation
    unsigned int id;
    char *type;
    char *path;
    VkImage textureImage;              // Raw image data in ram
    VkDeviceMemory textureImageMemory; // Image data in GPU/device ram
    VkImageView textureImageView;      // Texture format
    VkSampler textureSampler;          // Image sampling settings
};

// Forward declaration of Window struct
struct Window;

void texture_init(struct Texture *texture, char *path);
void texture_delete(struct Window *window, struct Texture *texture);

int create_texture_image(struct Window *window, struct Texture *texture);
int create_texture_sampler(struct Window *window, struct Texture *texture);
int create_texture_image_view(struct Window *window, struct Texture *texture);
VkImageView create_image_view(struct Window *window, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags);
int create_image(struct Window *window, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *image, VkDeviceMemory *image_memory);

VkFormat find_depth_format(struct Window *window);
int transition_image_layout(struct Window *window, VkImage *image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);

VkCommandBuffer begin_single_time_commands();
void end_single_time_commands(struct Window *window, VkCommandBuffer command_buffer);

int create_buffer(struct Window *window, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *buffer_memory);
void copy_buffer_to_image(struct Window *window, VkBuffer *buffer, VkImage *image, uint32_t width, uint32_t height);

uint32_t find_memory_type(struct Window *window, uint32_t typeFilter, VkMemoryPropertyFlags properties);

#endif // TEXTURE_H_
