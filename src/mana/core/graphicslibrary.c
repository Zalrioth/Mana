#include "mana/core/graphicslibrary.h"

int graphics_library_init(struct GraphicsLibrary* graphics_library, enum GraphicsLibraryType graphics_library_type) {
  graphics_library->type = graphics_library_type;
  switch (graphics_library->type) {
    case (NO_LIBRARY):
      break;
    case (GLFW_LIBRARY):
      glfw_init();
      break;
    case (MOLTENVK_LIBRARY):
      break;
  }

  return 1;
}

int glfw_init() {
  if (!glfwInit())
    return -1;

  if (!glfwVulkanSupported())
    return -1;

  return 1;
}

int glfw_delete() {
  glfwTerminate();

  return 1;
}
