#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include "mana/core/memoryallocator.h"
//
#include "mana/core/engine.h"
#include "mana/core/gpuapi.h"
#include "mana/core/graphicslibrary.h"
#include "mana/core/inputmanager.h"
#include "mana/core/vulkancore.h"
#include "mana/graphics/render/vulkanrenderer.h"

struct Engine;

struct Window {
  int width;
  int height;
  struct InputManager* input_manager;
  uint32_t image_index;
  struct Engine* engine;
};

enum {
  WINDOW_ERROR = 0,
  WINDOW_SUCCESS = 1
};

int window_init(struct Window* window, struct Engine* engine, int width, int height, int msaa_samples);
void window_delete(struct Window* window);
void window_set_title(struct Window* window, char* title);
bool window_should_close(struct Window* window);
void window_prepare_frame(struct Window* window);
void window_end_frame(struct Window* window);
int window_glfw_window_init(struct Window* window, struct Engine* engine, int width, int height);

#endif  // WINDOW_H