// Launches a window for rendering then closes window and exits program

#include <mana/core/memoryallocator.h>
//
#include <mana/mana.h>

int main(int argc, char* argv[]) {
  struct Mana mana = {0};
  mana_init(&mana, (struct EngineSettings){GLFW_LIBRARY, VULKAN_API});
  struct Window window = {0};
  window_init(&window, &mana.engine, 1280, 720);

  window_delete(&window);
  mana_cleanup(&mana);

  return 0;
}
