#include "mana/core/graphicslibrary.h"

int graphics_library_init(struct GraphicsLibrary* graphics_library, enum GraphicsLibraryType graphics_library_type) {
  graphics_library->type = graphics_library_type;
  switch (graphics_library->type) {
    case (NO_LIBRARY):
      break;
    case (GLFW_LIBRARY):
      int glfw_library_error_code = glfw_library_init();
      switch (glfw_library_error_code) {
        case (GLFW_SUCCESS):
          break;
        case (GLFW_INIT_ERROR):
          fprintf(stderr, "Failed to init glfw!\n");
          return GRAPHICS_LIBRARY_GLFW_ERROR;
        case (GLFW_VULKAN_SUPPORT_ERROR):
          fprintf(stderr, "Failed to find Vulkan support!\n");
          return GRAPHICS_LIBRARY_GLFW_ERROR;
      }
      break;
    case (MOLTENVK_LIBRARY):
      break;
  }

  return GRAPHICS_LIBRARY_SUCCESS;
}

static int glfw_library_init() {
  if (!glfwInit())
    return GLFW_INIT_ERROR;
  if (!glfwVulkanSupported()) {
    glfw_delete();
    return GLFW_VULKAN_SUPPORT_ERROR;
  }

  return GRAPHICS_LIBRARY_SUCCESS;
}

static void glfw_delete() {
  glfwTerminate();
}
