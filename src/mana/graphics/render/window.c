#include "mana/graphics/render/window.h"

// TODO: Condense functions
int window_init(struct Window *window, int width, int height) {
  memset(window, 0, sizeof(struct Window));

  window->input_manager = calloc(1, sizeof(struct InputManager));
  input_manager_init(window->input_manager);

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
  free(window->input_manager);
  vulkan_renderer_delete(&window->renderer.vulkan_renderer);
}

bool window_should_close(struct Window *window) {
  if (glfwWindowShouldClose(window->renderer.vulkan_renderer.glfw_window)) return true;

  return false;
}

void window_prepare_frame(struct Window *window) {
  struct VulkanRenderer *vulkan_renderer = &window->renderer.vulkan_renderer;

  glfwPollEvents();
  input_manager_process_input(window->input_manager, window);

  // Wait for swap chain presentation to finish
  VkResult result = vkWaitForFences(vulkan_renderer->device, 2, vulkan_renderer->swap_chain->in_flight_fences, VK_TRUE, UINT64_MAX);

  result = vkAcquireNextImageKHR(vulkan_renderer->device, vulkan_renderer->swap_chain->swap_chain_khr, UINT64_MAX, vulkan_renderer->swap_chain->image_available_semaphores[vulkan_renderer->swap_chain->current_frame], VK_NULL_HANDLE, &window->image_index);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreate_swap_chain(vulkan_renderer);
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    fprintf(stderr, "failed to acquire swap chain image!\n");
}

void window_end_frame(struct Window *window) {
  struct VulkanRenderer *vulkan_renderer = &window->renderer.vulkan_renderer;
  VkSubmitInfo swap_chain_submit_info = {0};
  swap_chain_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore wait_semaphores[VULKAN_WAIT_SEMAPHORES] = {vulkan_renderer->swap_chain->image_available_semaphores[vulkan_renderer->swap_chain->current_frame], vulkan_renderer->post_process->post_process_semaphores[vulkan_renderer->post_process->ping_pong ^ true]};
  VkPipelineStageFlags wait_stages[VULKAN_WAIT_SEMAPHORES] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  swap_chain_submit_info.waitSemaphoreCount = VULKAN_WAIT_SEMAPHORES;
  swap_chain_submit_info.pWaitSemaphores = wait_semaphores;
  swap_chain_submit_info.pWaitDstStageMask = wait_stages;

  swap_chain_submit_info.commandBufferCount = 1;
  swap_chain_submit_info.pCommandBuffers = &vulkan_renderer->swap_chain->swap_chain_command_buffers[window->image_index];

  VkSemaphore signal_semaphores[] = {vulkan_renderer->swap_chain->render_finished_semaphores[vulkan_renderer->swap_chain->current_frame]};
  swap_chain_submit_info.signalSemaphoreCount = 1;
  swap_chain_submit_info.pSignalSemaphores = signal_semaphores;

  vkResetFences(vulkan_renderer->device, 1, &vulkan_renderer->swap_chain->in_flight_fences[vulkan_renderer->swap_chain->current_frame]);

  VkResult result = vkQueueSubmit(vulkan_renderer->graphics_queue, 1, &swap_chain_submit_info, vulkan_renderer->swap_chain->in_flight_fences[vulkan_renderer->swap_chain->current_frame]);

  if (result != VK_SUCCESS)
    fprintf(stderr, "Error to submit draw command buffer!\n");

  VkPresentInfoKHR presentInfo = {0};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signal_semaphores;

  VkSwapchainKHR swapChains[] = {vulkan_renderer->swap_chain->swap_chain_khr};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  presentInfo.pImageIndices = &window->image_index;

  result = vkQueuePresentKHR(vulkan_renderer->present_queue, &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vulkan_renderer->framebuffer_resized) {
    vulkan_renderer->framebuffer_resized = false;
    recreate_swap_chain(vulkan_renderer);
  } else if (result != VK_SUCCESS)
    fprintf(stderr, "failed to present swap chain image!\n");

  vulkan_renderer->swap_chain->current_frame = (vulkan_renderer->swap_chain->current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}
