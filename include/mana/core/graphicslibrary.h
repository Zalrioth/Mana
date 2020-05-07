#pragma once
#ifndef WINDOW_LIBRARY_H
#define WINDOW_LIBRARY_H

#include "mana/core/memoryallocator.h"
//
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "mana/core/gpuapi.h"

enum GraphicsLibraryType { NO_LIBRARY = 0,
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

int graphics_library_init(struct GraphicsLibrary* graphics_library, enum GraphicsLibraryType graphics_library_type);
int glfw_init();
int glfw_delete();

#endif  // WINDOW_LIBRARY_H
