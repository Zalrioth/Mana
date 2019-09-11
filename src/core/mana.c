#include "core/mana.h"

int mana_init(struct Mana* mana) {
  int engineError = engine_init(&mana->engine);
  switch (engineError) {
    default:
      break;
    case (GLFW_ERROR):
      printf("Error initializing GLFW!\n");
      return ENGINE_ERROR;
    case (VULKAN_SUPPORT_ERROR):
      printf("Vulkan support not found!\n");
      return ENGINE_ERROR;
  }

  return 0;
}

int mana_new_window(struct Mana* mana, int width, int height) {
  if (window_init(&mana->engine.window, width, height) != NO_ERROR)
    return WINDOW_ERROR;

  return NO_ERROR;
}

void mana_update(struct Mana* mana) {
  engine_update(&mana->engine);
}

bool mana_should_close(struct Mana* mana) {
  if (glfwWindowShouldClose(mana->engine.window.renderer.vulkan_renderer.glfw_window)) return true;

  return false;
}

void mana_close_window(struct Mana* mana) {
  window_delete(&mana->engine.window);
}

void mana_cleanup(struct Mana* mana) {
  engine_delete(&mana->engine);
}

void mana_print_fps(struct Mana* mana) {
  printf("Target FPS: %lf\n", mana->engine.fps_counter.second_target_fps);
  printf("Average FPS: %lf\n", mana->engine.fps_counter.second_average_fps);
  printf("Draw FPS: %d\n", mana->engine.fps_counter.second_frames);
  printf("Update FPS: %d\n\n", mana->engine.fps_counter.second_updates);
}
