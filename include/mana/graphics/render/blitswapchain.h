#pragma once
#ifndef BLIT_SWAPCHAIN_H
#define BLIT_SWAPCHAIN_H

#include "mana/graphics/render/sprite.h"
#include "mana/graphics/shader.h"

struct BlitSwapchain {
  struct Sprite sprite;
};

int blit_swapchain_init(struct BlitSwapchain* blit_swapchain, struct VulkanRenderer* vulkan_renderer, struct Shader* shader);
void blit_swapchain_delete(struct BlitSwapchain* blit_swapchain, struct VulkanRenderer* vulkan_renderer);

#endif  // BLIT_SWAPCHAIN_H