#pragma once
#ifndef WINDOW_LIBRARY_H
#define WINDOW_LIBRARY_H

#include "mana/core/memoryallocator.h"
//
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "mana/core/gpuapi.h"

enum GRAPHICS_LIBRARY_STATUS {
  GRAPHICS_LIBRARY_SUCCESS = 0,
  GRAPHICS_LIBRARY_GLFW_ERROR,
  GRAPHICS_LIBRARY_LAST_ERROR
};

enum GLFW_LIBRARY_STATUS {
  GLFW_SUCCESS = 0,
  GLFW_INIT_ERROR,
  GLFW_VULKAN_SUPPORT_ERROR,
  GLFW_LAST_ERROR
};

enum GraphicsLibraryType {
  NO_LIBRARY = 0,
  GLFW_LIBRARY = 1,
  MOLTENVK_LIBRARY = 2
};

struct GLFWLibrary {
  GLFWwindow* glfw_window;
};

struct GraphicsLibrary {
  union {
    struct GLFWLibrary glfw_library;
  };
  enum GraphicsLibraryType type;
};

int graphics_library_init(struct GraphicsLibrary* graphics_library, enum GraphicsLibraryType graphics_library_type, const char** graphics_lbrary_extensions, uint32_t* graphics_library_extension_count);
void graphics_library_delete(struct GraphicsLibrary* graphics_library);

#endif  // WINDOW_LIBRARY_H
