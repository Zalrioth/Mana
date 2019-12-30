#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#include <time.h>
#include "mana/graphics/render/window.h"

#define FPS_COUNT 10

struct FPSCounter {
  int fps_past[FPS_COUNT];
  double limit_update_fps;
  // maybe cap fps at 144 for limit? 30, 60, 120, 144
  double limit_draw_fps;
  int input[128];
  double now_time, last_time, delta_time, timer;
  int frames;
  double last_render_time;
  int fps;
  double target_fps;
  double average_fps;
  int updates;

  // frame safe
  double second_target_fps;
  double second_average_fps;
  int second_frames;
  int second_updates;
};

struct Engine {
  struct Window window;
  struct FPSCounter fps_counter;
};

enum EngineStatus {
  ENGINE_GLFW_ERROR = 0,
  ENGINE_SUCCESS = 1,
  ENGINE_VULKAN_SUPPORT_ERROR = 2
};

int engine_init(struct Engine* engine);
void engine_delete(struct Engine* engine);
double get_time();

#endif  // ENGINE_H
