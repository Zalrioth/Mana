#include "mana/mana.h"

int mana_init(struct Mana* mana, struct EngineSettings engine_settings) {
  mana->engine.engine_settings = engine_settings;
  int engine_error = engine_init(&mana->engine, engine_settings);
  switch (engine_error) {
    case (ENGINE_SUCCESS):
      break;
    case (ENGINE_GRAPHICS_LIBRARY_ERROR):
      fprintf(stderr, "Error setting up engine graphics library!\n");
      return MANA_ENGINE_ERROR;
    case (ENGINE_GPU_API_ERROR):
      fprintf(stderr, "Error setting up engine GPU API!\n");
      return MANA_ENGINE_ERROR;
    default:
      fprintf(stderr, "Unknown engine error! Error code: %d\n", engine_error);
      break;
  }

  return MANA_SUCCESS;
}

void mana_cleanup(struct Mana* mana) {
  engine_delete(&mana->engine);
}
