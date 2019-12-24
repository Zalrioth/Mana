#include "mana/core/engine.h"

int engine_init(struct Engine* engine) {
  if (!glfwInit())
    return ENGINE_GLFW_ERROR;

  if (!glfwVulkanSupported())
    return ENGINE_VULKAN_SUPPORT_ERROR;

  engine->fps_counter.limit_update_fps = 1.0 / 60.0;
  // maybe cap fps at 144 for limit? 30, 60, 120, 144
  engine->fps_counter.delta_time = 0;
  engine->fps_counter.frames = 0;
  engine->fps_counter.updates = 0;
  engine->fps_counter.last_render_time = 0;

  engine->fps_counter.target_fps = glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;
  engine->fps_counter.limit_draw_fps = 1.0 / engine->fps_counter.target_fps;
  engine->fps_counter.fps = (int)engine->fps_counter.target_fps;
  engine->fps_counter.now_time = engine->fps_counter.last_time = engine->fps_counter.timer = get_time();
  for (int loopNum = 0; loopNum < FPS_COUNT; loopNum++)
    engine->fps_counter.fps_past[loopNum] = engine->fps_counter.fps;

  return ENGINE_SUCCESS;
}

void engine_delete(struct Engine* engine) {
}

void render(struct Engine* engine) {
}

double get_time() {
  struct timespec current_time;
  timespec_get(&current_time, TIME_UTC);

  //return (double)current_time.tv_sec * 1E6 + (double)current_time.tv_nsec;
  return (double)current_time.tv_sec + (double)current_time.tv_nsec / 1000000000;
}
