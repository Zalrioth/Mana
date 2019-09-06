#pragma once
#ifndef ENGINE_H_
#define ENGINE_H_

#include "core/common.h"
#include "core/window.h"

#define GLFW_ERROR 1
#define VULKAN_SUPPORT_ERROR 2

#define FPS_COUNT 10

#define KEY_LIMIT 512

enum KeyState { PRESSED, RELEASED };

struct Key {
  enum KeyState state;
  bool held;
};

struct FPSCounter {
  int fps_past[FPS_COUNT];
  double limit_update_fps;
  // maybe cap fps at 144 for limit? 30, 60, 120, 144
  double limit_draw_fps;
  int input[128];
  double last_time, timer;
  double delta_time;
  int now_time;
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
  struct Key keys[KEY_LIMIT];
};

double get_time();
int engine_init(struct Engine* engine);
void engine_delete(struct Engine* engine);
void engine_update(struct Engine* engine);
void logic(struct Engine* engine, double delta_time);
void process_input(struct Engine* engine);
void update_uniform_buffer(struct Window* window, uint32_t current_image);

#endif  // ENGINE_H_