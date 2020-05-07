#include "mana/graphics/render/window.h"

int window_init(struct Window *window, struct Engine *engine, int width, int height) {
  window->input_manager = calloc(1, sizeof(struct InputManager));
  input_manager_init(window->input_manager);

  window->width = width;
  window->height = height;
  window->engine = engine;

  switch (engine->graphics_library.type) {
    case (NO_LIBRARY):
      break;
    case (GLFW_LIBRARY):
      engine->fps_counter.limit_update_fps = 1.0 / 60.0;
      engine->fps_counter.delta_time = 0;
      engine->fps_counter.frames = 0;
      engine->fps_counter.updates = 0;
      engine->fps_counter.last_render_time = 0;

      engine->fps_counter.target_fps = glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;
      engine->fps_counter.limit_draw_fps = 1.0 / engine->fps_counter.target_fps;
      engine->fps_counter.fps = (int)engine->fps_counter.target_fps;
      engine->fps_counter.now_time = engine->fps_counter.last_time = engine->fps_counter.timer = engine_get_time();
      for (int loopNum = 0; loopNum < FPS_COUNT; loopNum++)
        engine->fps_counter.fps_past[loopNum] = engine->fps_counter.fps;
      window_glfw_window_init(window, engine, width, height);
      break;
    case (MOLTENVK_LIBRARY):
      break;
  }

  switch (engine->gpu_api.type) {
    case (VULKAN_API):
      switch (vulkan_renderer_init(engine->gpu_api.vulkan_state, width, height)) {
        default:
          return WINDOW_SUCCESS;
          break;
        case (VULKAN_CORE_CREATE_WINDOW_ERROR):
          printf("Error creating GLFW window!\n");
          return WINDOW_ERROR;
        case (VULKAN_CORE_CREATE_INSTANCE_ERROR):
          printf("Failed to create Vulkan instance!\n");
          return WINDOW_ERROR;
        case (VULKAN_CORE_SETUP_DEBUG_MESSENGER_ERROR):
          printf("Failed to set up debug messengerS!\n");
          return WINDOW_ERROR;
        case (VULKAN_CORE_CREATE_SURFACE_ERROR):
          printf("Failed to create window surface!\n");
          return WINDOW_ERROR;
        case (VULKAN_CORE_PICK_PHYSICAL_DEVICE_ERROR):
          printf("Failed to find a suitable GPU!\n");
          return WINDOW_ERROR;
        case (VULKAN_CORE_CREATE_LOGICAL_DEVICE_ERROR):
          printf("Failed to create logical device!\n");
          return WINDOW_ERROR;
        case (VULKAN_CORE_CREATE_SWAP_CHAIN_ERROR):
          printf("Failed to create swap chain!\n");
          return WINDOW_ERROR;
        case (VULKAN_CORE_CREATE_IMAGE_VIEWS_ERROR):
          printf("Failed to create image views!\n");
          return WINDOW_ERROR;
        case (VULKAN_CORE_CREATE_RENDER_PASS_ERROR):
          printf("Failed to create render pass!\n");
          return WINDOW_ERROR;
        case (VULKAN_CORE_CREATE_GRAPHICS_PIPELINE_ERROR):
          printf("Failed to create pipeline layout!\n");
          return WINDOW_ERROR;
        case (VULKAN_CORE_CREATE_FRAME_BUFFER_ERROR):
          printf("Failed to create framebuffer!\n");
          return WINDOW_ERROR;
        case (VULKAN_CORE_CREATE_COMMAND_POOL_ERROR):
          printf("Failed to create command pool!\n");
          return WINDOW_ERROR;
        case (VULKAN_CORE_CREATE_COMMAND_BUFFER_ERROR):
          printf("Failed to begin recording command buffer!\n");
          return WINDOW_ERROR;
        case (VULKAN_CORE_CREATE_SYNC_OBJECT_ERROR):
          printf("Failed to create synchronization objects for a frame!\n");
          return WINDOW_ERROR;
      }
      break;
  }
}

void window_delete(struct Window *window) {
  glfwDestroyWindow(window->engine->graphics_library.glfw_library.glfw_window);
  free(window->input_manager);
  //vulkan_renderer_delete(&window->renderer.vulkan_core);
}

void window_set_title(struct Window *window, char *title) {
  switch (window->engine->graphics_library.type) {
    case (NO_LIBRARY):
      break;
    case (GLFW_LIBRARY):
      glfwSetWindowTitle(window->engine->graphics_library.glfw_library.glfw_window, title);
      break;
    case (MOLTENVK_LIBRARY):
      break;
  }
}

bool window_should_close(struct Window *window) {
  if (glfwWindowShouldClose(window->engine->graphics_library.glfw_library.glfw_window)) return true;

  return false;
}

void window_prepare_frame(struct Window *window) {
  struct VulkanState *vulkan_core = window->engine->gpu_api.vulkan_state;

  glfwPollEvents();
  input_manager_process_input(window->input_manager, window);

  // Wait for swap chain presentation to finish
  VkResult result = vkWaitForFences(vulkan_core->device, 2, vulkan_core->swap_chain->in_flight_fences, VK_TRUE, UINT64_MAX);

  result = vkAcquireNextImageKHR(vulkan_core->device, vulkan_core->swap_chain->swap_chain_khr, UINT64_MAX, vulkan_core->swap_chain->image_available_semaphores[vulkan_core->swap_chain->current_frame], VK_NULL_HANDLE, &window->image_index);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    //recreate_swap_chain(vulkan_core);
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    fprintf(stderr, "failed to acquire swap chain image!\n");
}

void window_end_frame(struct Window *window) {
  struct VulkanState *vulkan_core = window->engine->gpu_api.vulkan_state;
  VkSubmitInfo swap_chain_submit_info = {0};
  swap_chain_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore wait_semaphores[VULKAN_WAIT_SEMAPHORES] = {vulkan_core->swap_chain->image_available_semaphores[vulkan_core->swap_chain->current_frame], vulkan_core->post_process->post_process_semaphores[vulkan_core->post_process->ping_pong ^ true]};
  VkPipelineStageFlags wait_stages[VULKAN_WAIT_SEMAPHORES] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  swap_chain_submit_info.waitSemaphoreCount = VULKAN_WAIT_SEMAPHORES;
  swap_chain_submit_info.pWaitSemaphores = wait_semaphores;
  swap_chain_submit_info.pWaitDstStageMask = wait_stages;

  swap_chain_submit_info.commandBufferCount = 1;
  swap_chain_submit_info.pCommandBuffers = &vulkan_core->swap_chain->swap_chain_command_buffers[window->image_index];

  VkSemaphore signal_semaphores[] = {vulkan_core->swap_chain->render_finished_semaphores[vulkan_core->swap_chain->current_frame]};
  swap_chain_submit_info.signalSemaphoreCount = 1;
  swap_chain_submit_info.pSignalSemaphores = signal_semaphores;

  vkResetFences(vulkan_core->device, 1, &vulkan_core->swap_chain->in_flight_fences[vulkan_core->swap_chain->current_frame]);

  VkResult result = vkQueueSubmit(vulkan_core->graphics_queue, 1, &swap_chain_submit_info, vulkan_core->swap_chain->in_flight_fences[vulkan_core->swap_chain->current_frame]);

  if (result != VK_SUCCESS)
    fprintf(stderr, "Error to submit draw command buffer!\n");

  VkPresentInfoKHR presentInfo = {0};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signal_semaphores;

  VkSwapchainKHR swapChains[] = {vulkan_core->swap_chain->swap_chain_khr};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  presentInfo.pImageIndices = &window->image_index;

  result = vkQueuePresentKHR(vulkan_core->present_queue, &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vulkan_core->framebuffer_resized) {
    vulkan_core->framebuffer_resized = false;
    //recreate_swap_chain(vulkan_core);
  } else if (result != VK_SUCCESS)
    fprintf(stderr, "failed to present swap chain image!\n");

  vulkan_core->swap_chain->current_frame = (vulkan_core->swap_chain->current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

static void glfw_framebuffer_resize_callback(GLFWwindow *window, int width, int height) {
  struct VulkanState *vulkan_state_handle = (struct VulkanState *)(glfwGetWindowUserPointer(window));
  vulkan_state_handle->framebuffer_resized = true;
}

int window_glfw_window_init(struct Window *window, struct Engine *engine, int width, int height) {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  engine->graphics_library.glfw_library.glfw_window = glfwCreateWindow(width, height, "Grindstone", NULL, NULL);

  //glfwSetWindowUserPointer(engine->vulkan_api->glfw_window, &vulkan_api);
  glfwSetWindowUserPointer(engine->graphics_library.glfw_library.glfw_window, engine->gpu_api.vulkan_state);
  glfwSetFramebufferSizeCallback(engine->graphics_library.glfw_library.glfw_window, glfw_framebuffer_resize_callback);

  glfwSwapInterval(1);
  glfwMakeContextCurrent(engine->graphics_library.glfw_library.glfw_window);

  if (!engine->graphics_library.glfw_library.glfw_window)
    return VULKAN_CORE_CREATE_WINDOW_ERROR;

  if (glfwCreateWindowSurface(engine->gpu_api.vulkan_state->instance, engine->graphics_library.glfw_library.glfw_window, NULL, &engine->gpu_api.vulkan_state->surface) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_SURFACE_ERROR;

  return 1;
}
