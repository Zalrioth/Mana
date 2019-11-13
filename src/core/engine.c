#include "core/engine.h"

int engine_init(struct Engine* engine) {
  if (!glfwInit())
    return ENGINE_GLFW_ERROR;

  if (!glfwVulkanSupported())
    return ENGINE_VULKAN_SUPPORT_ERROR;

  memset(engine->keys, RELEASED, sizeof(engine->keys));

  engine->fps_counter.limit_update_fps = 1.0 / 60.0;
  // maybe cap fps at 144 for limit? 30, 60, 120, 144
  engine->fps_counter.delta_time = 0;
  engine->fps_counter.now_time = 0;
  engine->fps_counter.frames = 0;
  engine->fps_counter.updates = 0;
  engine->fps_counter.last_render_time = 0;

  engine->fps_counter.target_fps = glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;
  engine->fps_counter.limit_draw_fps = 1.0 / engine->fps_counter.target_fps;
  engine->fps_counter.fps = (int)engine->fps_counter.target_fps;
  engine->fps_counter.last_time = get_time();
  engine->fps_counter.timer = engine->fps_counter.last_time;
  for (int loopNum = 0; loopNum < FPS_COUNT; loopNum++)
    engine->fps_counter.fps_past[loopNum] = engine->fps_counter.fps;

  return ENGINE_SUCCESS;
}

void engine_delete(struct Engine* engine) {
}

void render(struct Engine* engine) {
}

double get_time() {
  struct timespec currentTime;
  timespec_get(&currentTime, TIME_UTC);

  return (double)currentTime.tv_sec + (double)currentTime.tv_nsec / 1000000000;
}

void engine_process_input(struct Engine* engine) {
  for (int loopNum = 0; loopNum < KEY_LIMIT; loopNum++) {
    if (glfwGetKey(engine->window.renderer.vulkan_renderer.glfw_window, loopNum) == GLFW_PRESS) {
      engine->keys[loopNum].state = PRESSED;
      engine->keys[loopNum].held = true;
    } else if (glfwGetKey(engine->window.renderer.vulkan_renderer.glfw_window, loopNum) == GLFW_RELEASE) {
      engine->keys[loopNum].state = RELEASED;
      engine->keys[loopNum].held = false;
    }
  }
}
