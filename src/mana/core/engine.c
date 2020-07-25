#include "mana/core/engine.h"

int engine_init(struct Engine* engine, struct EngineSettings engine_settings) {
  engine->engine_settings = engine_settings;

  const char* graphics_lbrary_extensions[MAX_GRAPHICS_LIBRARY_EXTENSIONS] = {0};
  uint32_t graphics_library_extension_count;

  int graphics_library_error = graphics_library_init(&engine->graphics_library, engine_settings.graphics_library_type, graphics_lbrary_extensions, &graphics_library_extension_count);
  switch (graphics_library_error) {
    case (GRAPHICS_LIBRARY_SUCCESS):
      break;
    case (GRAPHICS_LIBRARY_GLFW_ERROR):
      fprintf(stderr, "Failed to setup glfw for engine!\n");
      return ENGINE_GRAPHICS_LIBRARY_ERROR;
    default:
      fprintf(stderr, "Unknown graphics library error! Error code: %d\n", graphics_library_error);
      return ENGINE_GRAPHICS_LIBRARY_ERROR;
  }

  int gpu_api_error = gpu_api_init(&engine->gpu_api, engine_settings.gpu_api_type, &engine->graphics_library, graphics_lbrary_extensions, &graphics_library_extension_count);
  switch (gpu_api_error) {
    case (GPU_API_SUCCESS):
      break;
    case (GPU_API_VULKAN_ERROR):
      fprintf(stderr, "Failed to setup Vulkan API for engine!\n");
      return ENGINE_GPU_API_ERROR;
    default:
      fprintf(stderr, "Unknown gpu api error! Error code: %d\n", gpu_api_error);
      return ENGINE_GPU_API_ERROR;
  }

  return ENGINE_SUCCESS;
}

void engine_delete(struct Engine* engine) {
  gpu_api_delete(&engine->gpu_api);
  graphics_library_delete(&engine->graphics_library);
}

double engine_get_time() {
  struct timespec current_time;
  timespec_get(&current_time, TIME_UTC);
  return (double)current_time.tv_sec + (double)current_time.tv_nsec / 1000000000;
}
