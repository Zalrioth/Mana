#include "mana/core/engine.h"

int engine_init(struct Engine* engine, struct EngineSettings engine_settings) {
  engine->engine_settings = engine_settings;

  int engine_error_code;
  // init graphics library if using
  graphics_library_init(&engine->graphics_library, engine_settings.graphics_library_type);
  // init gpu api
  gpu_api_init(&engine->gpu_api, engine_settings.gpu_api_type, &engine->graphics_library);

  return ENGINE_SUCCESS;
}

void engine_delete(struct Engine* engine) {
}

void render(struct Engine* engine) {
}

double engine_get_time() {
  struct timespec current_time;
  timespec_get(&current_time, TIME_UTC);
  return (double)current_time.tv_sec + (double)current_time.tv_nsec / 1000000000;
}
