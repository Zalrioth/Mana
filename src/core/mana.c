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
  switch (window_init(&mana->engine.window, width, height)) {
    default:
      break;
    case (CREATE_WINDOW_ERROR):
      printf("Error creating GLFW window!\n");
      return WINDOW_ERROR;
    case (CREATE_INSTANCE_ERROR):
      printf("Failed to create Vulkan instance!\n");
      return WINDOW_ERROR;
    case (SETUP_DEBUG_MESSENGER_ERROR):
      printf("Failed to set up debug messengerS!\n");
      return WINDOW_ERROR;
    case (CREATE_SURFACE_ERROR):
      printf("Failed to create window surface!\n");
      return WINDOW_ERROR;
    case (PICK_PHYSICAL_DEVICE_ERROR):
      printf("Failed to find a suitable GPU!\n");
      return WINDOW_ERROR;
    case (CREATE_LOGICAL_DEVICE_ERROR):
      printf("Failed to create logical device!\n");
      return WINDOW_ERROR;
    case (CREATE_SWAP_CHAIN_ERROR):
      printf("Failed to create swap chain!\n");
      return WINDOW_ERROR;
    case (CREATE_IMAGE_VIEWS_ERROR):
      printf("Failed to create image views!\n");
      return WINDOW_ERROR;
    case (CREATE_RENDER_PASS_ERROR):
      printf("Failed to create render pass!\n");
      return WINDOW_ERROR;
    case (CREATE_GRAPHICS_PIPELINE_ERROR):
      printf("Failed to create pipeline layout!\n");
      return WINDOW_ERROR;
    case (CREATE_FRAME_BUFFER_ERROR):
      printf("Failed to create framebuffer!\n");
      return WINDOW_ERROR;
    case (CREATE_COMMAND_POOL_ERROR):
      printf("Failed to create command pool!\n");
      return WINDOW_ERROR;
    case (CREATE_COMMAND_BUFFER_ERROR):
      printf("Failed to begin recording command buffer!\n");
      return WINDOW_ERROR;
    case (CREATE_SYNC_OBJECT_ERROR):
      printf("Failed to create synchronization objects for a frame!\n");
      return WINDOW_ERROR;
  }

  return NO_ERROR;
}

void mana_update(struct Mana* mana) {
  engine_update(&mana->engine);
}

bool mana_should_close(struct Mana* mana) {
  if (glfwWindowShouldClose(mana->engine.window.glfw_window)) return true;

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
