#include "mana/graphics/utilities/texturecache.h"

void texture_cache_init(struct TextureCache* texture_cache) {
  // Note: Store as references because it would be dangerous to realloc in linear memory
  map_init(&texture_cache->textures, sizeof(struct Texture*));
}

void texture_cache_delete(struct TextureCache* texture_cache, struct GPUAPI* gpu_api) {
  const char* texture_key;
  struct MapIter texture_iter = map_iter();
  while ((texture_key = map_next(&texture_cache->textures, &texture_iter))) {
    struct Texture* texture = *(struct Texture**)map_get(&texture_cache->textures, texture_key);
    texture_delete(texture, gpu_api);
    free(texture);
  }

  map_delete(&texture_cache->textures);
}

void texture_cache_add(struct TextureCache* texture_cache, struct GPUAPI* gpu_api, size_t n_textures, ...) {
  va_list args;
  va_start(args, n_textures);

  while (n_textures-- > 0) {
    struct TextureSettings texture_settings = va_arg(args, struct TextureSettings);
    struct Texture* texture = malloc(sizeof(struct Texture));
    texture_init(texture, gpu_api, texture_settings);
    map_set(&texture_cache->textures, texture_settings.path, &texture);  // Store full path in case of models having same texture name like diffuse
  }

  va_end(args);
}

struct Texture* texture_cache_get(struct TextureCache* texture_cache, char* texture_name) {
  return *((struct Texture**)map_get(&texture_cache->textures, texture_name));
}
