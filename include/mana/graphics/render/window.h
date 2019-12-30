#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include "mana/core/inputmanager.h"
#include "mana/graphics/render/vulkanrenderer.h"

enum RendererType { VULKAN };

union Renderer {
  struct VulkanRenderer vulkan_renderer;
};

struct Window {
  int width;
  int height;
  enum RendererType renderer_type;
  union Renderer renderer;
  struct InputManager* input_manager;
  uint32_t image_index;
};

enum {
  WINDOW_ERROR = 0,
  WINDOW_SUCCESS = 1
};

int window_init(struct Window* window, int width, int height);
void window_delete(struct Window* game_window);
bool window_should_close(struct Window* window);
void window_prepare_frame(struct Window* window);
void window_end_frame(struct Window* window);

#endif  // WINDOW_H