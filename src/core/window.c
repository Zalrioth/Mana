#include "core/window.h"

// TODO: Condense functions
int window_init(struct Window *window, int width, int height) {
  memset(window, 0, sizeof(struct Window));

  window->width = width;
  window->height = height;

  switch (vulkan_renderer_init(&window->renderer.vulkan_renderer, width, height)) {
    default:
      return WINDOW_SUCCESS;
      break;
    case (VULKAN_RENDERER_CREATE_WINDOW_ERROR):
      printf("Error creating GLFW window!\n");
      return WINDOW_ERROR;
    case (VULKAN_RENDERER_CREATE_INSTANCE_ERROR):
      printf("Failed to create Vulkan instance!\n");
      return WINDOW_ERROR;
    case (VULKAN_RENDERER_SETUP_DEBUG_MESSENGER_ERROR):
      printf("Failed to set up debug messengerS!\n");
      return WINDOW_ERROR;
    case (VULKAN_RENDERER_CREATE_SURFACE_ERROR):
      printf("Failed to create window surface!\n");
      return WINDOW_ERROR;
    case (VULKAN_RENDERER_PICK_PHYSICAL_DEVICE_ERROR):
      printf("Failed to find a suitable GPU!\n");
      return WINDOW_ERROR;
    case (VULKAN_RENDERER_CREATE_LOGICAL_DEVICE_ERROR):
      printf("Failed to create logical device!\n");
      return WINDOW_ERROR;
    case (VULKAN_RENDERER_CREATE_SWAP_CHAIN_ERROR):
      printf("Failed to create swap chain!\n");
      return WINDOW_ERROR;
    case (VULKAN_RENDERER_CREATE_IMAGE_VIEWS_ERROR):
      printf("Failed to create image views!\n");
      return WINDOW_ERROR;
    case (VULKAN_RENDERER_CREATE_RENDER_PASS_ERROR):
      printf("Failed to create render pass!\n");
      return WINDOW_ERROR;
    case (VULKAN_RENDERER_CREATE_GRAPHICS_PIPELINE_ERROR):
      printf("Failed to create pipeline layout!\n");
      return WINDOW_ERROR;
    case (VULKAN_RENDERER_CREATE_FRAME_BUFFER_ERROR):
      printf("Failed to create framebuffer!\n");
      return WINDOW_ERROR;
    case (VULKAN_RENDERER_CREATE_COMMAND_POOL_ERROR):
      printf("Failed to create command pool!\n");
      return WINDOW_ERROR;
    case (VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR):
      printf("Failed to begin recording command buffer!\n");
      return WINDOW_ERROR;
    case (VULKAN_RENDERER_CREATE_SYNC_OBJECT_ERROR):
      printf("Failed to create synchronization objects for a frame!\n");
      return WINDOW_ERROR;
  }
}

void window_delete(struct Window *window) {
  vulkan_renderer_delete(&window->renderer.vulkan_renderer);
}

bool window_should_close(struct Window *window) {
  if (glfwWindowShouldClose(window->renderer.vulkan_renderer.glfw_window)) return true;

  return false;
}
