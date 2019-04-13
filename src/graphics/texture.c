#include "graphics/texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <graphics/stb_image.h>

void init_texture(struct Window* window, struct Texture* texture)
{
}

void delete_texture(struct Window* window, struct Texture* texture)
{
    vkDestroySampler(window->device, texture->textureSampler, NULL);
    vkDestroyImageView(window->device, texture->textureImageView, NULL);

    vkDestroyImage(window->device, texture->textureImage, NULL);
    vkFreeMemory(window->device, texture->textureImageMemory, NULL);
}

int createTextureImage(struct Window* window, struct Texture* texture)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("./Assets/textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels)
        return -1;
    //printf("failed to load texture image!\n");

    VkBuffer stagingBuffer = { 0 };
    VkDeviceMemory stagingBufferMemory = { 0 };
    createBuffer(window, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(window->device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, imageSize);
    vkUnmapMemory(window->device, stagingBufferMemory);

    stbi_image_free(pixels);

    createImage(window, texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texture->textureImage, &texture->textureImageMemory);

    transitionImageLayout(window, &texture->textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(window, &stagingBuffer, &texture->textureImage, texWidth, texHeight);
    transitionImageLayout(window, &texture->textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(window->device, stagingBuffer, NULL);
    vkFreeMemory(window->device, stagingBufferMemory, NULL);

    return 0;
}

int createTextureSampler(struct Window* window, struct Texture* texture)
{
    VkSamplerCreateInfo samplerInfo = { 0 };
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

    if (vkCreateSampler(window->device, &samplerInfo, NULL, &texture->textureSampler) != VK_SUCCESS)
        return -1;
    //printf("failed to create texture sampler!\n");

    return 0;
}

int createTextureImageView(struct Window* window)
{
    window->textureImageView = createImageView(window, window->textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

    return 0;
}
