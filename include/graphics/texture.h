#pragma once
#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "core/common.h"

struct Texture {
    unsigned int id;
    char* type;
    char* path;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
};

void init_texture();
void delete_texture();

#endif // TEXTURE_H_
