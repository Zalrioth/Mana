#include "mana/core/graphicslibrary.h"

static int glfw_library_init(const char** graphics_lbrary_extensions, uint32_t* graphics_library_extension_count);
static void glfw_delete();

int graphics_library_init(struct GraphicsLibrary* graphics_library, enum GraphicsLibraryType graphics_library_type, const char** graphics_lbrary_extensions, uint32_t* graphics_library_extension_count) {
  graphics_library->type = graphics_library_type;
  switch (graphics_library->type) {
    case (NO_LIBRARY):
      break;
    case (GLFW_LIBRARY):
      int glfw_library_error_code = glfw_library_init(graphics_lbrary_extensions, graphics_library_extension_count);
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

void graphics_library_delete(struct GraphicsLibrary* graphics_library) {
  switch (graphics_library->type) {
    case (NO_LIBRARY):
      break;
    case (GLFW_LIBRARY):
      glfw_delete();
      break;
  }
}

static int glfw_library_init(const char** graphics_lbrary_extensions, uint32_t* graphics_library_extension_count) {
  if (!glfwInit())
    return GLFW_INIT_ERROR;
  if (!glfwVulkanSupported()) {
    glfw_delete();
    return GLFW_VULKAN_SUPPORT_ERROR;
  }

  const char** glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(graphics_library_extension_count);

  for (int extension_num = 0; extension_num < *graphics_library_extension_count; extension_num++)
    graphics_lbrary_extensions[extension_num] = glfw_extensions[extension_num];

  return GRAPHICS_LIBRARY_SUCCESS;
}

static void glfw_delete() {
  glfwTerminate();
}
