#include "mana/graphics/utilities/texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int texture_init(struct Texture *texture, struct GPUAPI *gpu_api, struct TextureSettings texture_settings) {
  VkFilter filter = (texture_settings.filter_type == FILTER_NEAREST) ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;
  VkSamplerAddressMode mode;
  switch (texture_settings.mode_type) {
    case (MODE_REPEAT):
      mode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      break;
    case (MODE_MIRRORED_REPEAT):
      mode = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
      break;
    case (MODE_CLAMP_TO_EDGE):
      mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      break;
    case (MODE_CLAMP_TO_BORDER):
      mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
      break;
  }
  //texture->filter_type = filter;
  texture->path = strdup(texture_settings.path);

  char *name_location = strrchr(texture_settings.path, '/');
  if (!name_location)
    texture->name = strdup(name_location);
  else
    texture->name = strdup(name_location + 1);

  char *type_location = strrchr(texture_settings.path, '.');
  if (!type_location)
    texture->type = strdup(type_location);
  else
    texture->type = strdup(type_location + 1);

  // Todo: Detect pixel bit

  // Note: Something like this could be useful for optimizing but not needed as stbi will correctly convert up/down bits
  //int pixel_bit = 16;
  //int tex_width, tex_height, tex_channels;
  //void *pixels;
  //VkDeviceSize image_size;
  //if (pixel_bit == 8) {
  //  pixels = (void *)stbi_load(texture->path, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
  //  image_size = tex_width * tex_height * 4;
  //} else if (pixel_bit == 16) {
  //  pixels = (void *)stbi_load_16(texture->path, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
  //  image_size = tex_width * tex_height * 4 * 2;
  //} else if (pixel_bit == 32) {
  //  pixels = (void *)stbi_load_32(texture->path, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
  //  image_size = tex_width * tex_height * 4 * 2 * 2;
  //} else
  //  return -1;

  int tex_width, tex_height, tex_channels;
  stbi_us *pixels = stbi_load_16(texture->path, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
  VkDeviceSize image_size = tex_width * tex_height * 4 * 2;

  texture->width = tex_width;
  texture->height = tex_height;

  if (!pixels) {
    printf("failed to load texture image!\n");
    return -1;
  }

  // Guaranteed to be multiple of 4
  // ifndef avx512 -> ifndef avx2 -> ifndef avx -> ifndef sse2 -> ifndef neon -> fallback
  if (texture_settings.premultiplied_alpha == 0) {
// Note: Let openmp take care of simd for now, outside of texture uses parallel for inside used parallel simd
//https://stackoverflow.com/questions/14674049/parallel-for-vs-omp-simd-when-to-use-each
#pragma omp simd
    for (int pixel_group_num = 0; pixel_group_num < tex_width * tex_height * tex_channels; pixel_group_num += 4) {
      unsigned short alpha_value = pixels[pixel_group_num + 3];
      pixels[pixel_group_num] *= ((float)alpha_value / USHRT_MAX);
      pixels[pixel_group_num + 1] *= ((float)alpha_value / USHRT_MAX);
      pixels[pixel_group_num + 2] *= ((float)alpha_value / USHRT_MAX);
    }
  }
  //  int total_pixels = tex_width * tex_height * tex_channels;
  //#ifdef __AVX__
  //  for (int iterations = total_pixels / 4;;)
  //    ;
  //  if (total_pixels % 4 != 0) {
  //    unsigned short alpha_value = pixels[total_pixels - 1];
  //    pixels[total_pixels - 4] *= ((float)alpha_value / USHRT_MAX);
  //    pixels[total_pixels - 3] *= ((float)alpha_value / USHRT_MAX);
  //    pixels[total_pixels - 2] *= ((float)alpha_value / USHRT_MAX);
  //  }
  //#else
  //  if (texture_settings.premultiplied_alpha == 0) {
  //#pragma omp parallel for
  //    for (int pixel_group_num = 0; pixel_group_num < tex_width * tex_height * tex_channels; pixel_group_num += 4) {
  //      unsigned short alpha_value = pixels[pixel_group_num + 3];
  //      pixels[pixel_group_num] *= ((float)alpha_value / USHRT_MAX);
  //      pixels[pixel_group_num + 1] *= ((float)alpha_value / USHRT_MAX);
  //      pixels[pixel_group_num + 2] *= ((float)alpha_value / USHRT_MAX);
  //    }
  //  }
  //#endif
  VkBuffer staging_buffer = {0};
  VkDeviceMemory staging_buffer_memory = {0};
  graphics_utils_create_buffer(gpu_api->vulkan_state->device, gpu_api->vulkan_state->physical_device, image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer, &staging_buffer_memory);

  void *data;
  vkMapMemory(gpu_api->vulkan_state->device, staging_buffer_memory, 0, image_size, 0, &data);
  memcpy(data, pixels, image_size);
  vkUnmapMemory(gpu_api->vulkan_state->device, staging_buffer_memory);

  stbi_image_free(pixels);

  uint32_t mip_levels = (uint32_t)(floor(log2(MAX(tex_width, tex_height))));
  if (texture_settings.mip_maps_enabled == 0)
    mip_levels = 1;

  graphics_utils_create_image(gpu_api->vulkan_state->device, gpu_api->vulkan_state->physical_device, tex_width, tex_height, mip_levels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R16G16B16A16_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texture->texture_image, &texture->texture_image_memory);

  graphics_utils_transition_image_layout(gpu_api->vulkan_state->device, gpu_api->vulkan_state->graphics_queue, gpu_api->vulkan_state->command_pool, texture->texture_image, VK_FORMAT_R16G16B16A16_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mip_levels);
  graphics_utils_copy_buffer_to_image(gpu_api->vulkan_state->device, gpu_api->vulkan_state->graphics_queue, gpu_api->vulkan_state->command_pool, &staging_buffer, &texture->texture_image, tex_width, tex_height);

  vkDestroyBuffer(gpu_api->vulkan_state->device, staging_buffer, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, staging_buffer_memory, NULL);

  graphics_utils_generate_mipmaps(gpu_api->vulkan_state->device, gpu_api->vulkan_state->physical_device, gpu_api->vulkan_state->graphics_queue, gpu_api->vulkan_state->command_pool, texture->texture_image, VK_FORMAT_R16G16B16A16_UNORM, tex_width, tex_height, mip_levels);

  graphics_utils_create_image_view(gpu_api->vulkan_state->device, texture->texture_image, VK_FORMAT_R16G16B16A16_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, mip_levels, &texture->texture_image_view);
  graphics_utils_create_sampler(gpu_api->vulkan_state->device, &texture->texture_sampler, (struct SamplerSettings){.mip_levels = mip_levels, .filter = filter, .address_mode = mode});

  return 0;
}

void texture_delete(struct Texture *texture, struct GPUAPI *gpu_api) {
  vkDestroySampler(gpu_api->vulkan_state->device, texture->texture_sampler, NULL);
  vkDestroyImageView(gpu_api->vulkan_state->device, texture->texture_image_view, NULL);

  vkDestroyImage(gpu_api->vulkan_state->device, texture->texture_image, NULL);
  vkFreeMemory(gpu_api->vulkan_state->device, texture->texture_image_memory, NULL);

  free(texture->path);
  free(texture->name);
  free(texture->type);
}
