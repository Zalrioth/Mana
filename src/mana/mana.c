#include "mana/mana.h"

int mana_init(struct Mana* mana, struct EngineSettings engine_settings) {
  mana->engine.engine_settings = engine_settings;
  int engine_error = engine_init(&mana->engine, engine_settings);
  switch (engine_error) {
    default:
      break;
    case (ENGINE_GLFW_ERROR):
      printf("Error initializing GLFW!\n");
      return MANA_ENGINE_ERROR;
    case (ENGINE_VULKAN_SUPPORT_ERROR):
      printf("Vulkan support not found!\n");
      return MANA_ENGINE_ERROR;
  }

  return MANA_SUCCESS;
}

void mana_cleanup(struct Mana* mana) {
  engine_delete(&mana->engine);
}
