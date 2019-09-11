#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include "core/vulkanrenderer.h"

enum RendererType { VULKAN };

union Renderer {
  struct VulkanRenderer vulkan_renderer;
};

struct Window {
  int width;
  int height;
  enum RendererType renderer_type;
  union Renderer renderer;
};

int window_init(struct Window* window, int width, int height);
void window_delete(struct Window* game_window);

#endif  // WINDOW_H