#include "core/engine.h"

int engine_init(struct Engine* engine) {
  if (!glfwInit())
    return GLFW_ERROR;

  if (!glfwVulkanSupported())
    return VULKAN_SUPPORT_ERROR;

  memset(engine->keys, RELEASED, sizeof(engine->keys));

  engine->fps_counter.limit_update_fps = 1.0 / 60.0;
  // maybe cap fps at 144 for limit? 30, 60, 120, 144
  engine->fps_counter.delta_time = 0;
  engine->fps_counter.now_time = 0;
  engine->fps_counter.frames = 0;
  engine->fps_counter.updates = 0;
  engine->fps_counter.last_render_time = 0;

  engine->fps_counter.target_fps = glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;
  engine->fps_counter.limit_draw_fps = 1.0 / engine->fps_counter.target_fps;
  engine->fps_counter.fps = (int)engine->fps_counter.target_fps;
  engine->fps_counter.last_time = get_time();
  engine->fps_counter.timer = engine->fps_counter.last_time;
  for (int loopNum = 0; loopNum < FPS_COUNT; loopNum++)
    engine->fps_counter.fps_past[loopNum] = engine->fps_counter.fps;

  array_list_init(&engine->entities);

  return 0;
}

void engine_delete(struct Engine* engine) {
  struct VulkanRenderer* vulkan_renderer = &engine->window.renderer.vulkan_renderer;
  // Wait for command buffers to finish before deleting desciptor sets
  vkWaitForFences(vulkan_renderer->device, 2, vulkan_renderer->in_flight_fences, VK_TRUE, UINT64_MAX);
  for (int entity_num = 0; entity_num < array_list_size(&engine->entities); entity_num++) {
    sprite_delete((struct Sprite*)array_list_get(&engine->entities, entity_num), vulkan_renderer);
  }

  array_list_delete(&engine->entities);
}

void render(struct Engine* engine) {
  struct Window* window = &engine->window;
  struct VulkanRenderer* vulkan_renderer = &window->renderer.vulkan_renderer;
  VkResult result = vkWaitForFences(vulkan_renderer->device, 2, vulkan_renderer->in_flight_fences, VK_TRUE, UINT64_MAX);  //vkWaitForFences(vulkan_renderer->device, 1, &vulkan_renderer->in_flight_fences[vulkan_renderer->current_frame], VK_TRUE, UINT64_MAX);
  //vkResetCommandPool(vulkan_renderer->device, vulkan_renderer->command_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

  uint32_t imageIndex;
  result = vkAcquireNextImageKHR(vulkan_renderer->device, vulkan_renderer->swap_chain, UINT64_MAX, vulkan_renderer->image_available_semaphores[vulkan_renderer->current_frame], VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreate_swap_chain(vulkan_renderer);
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    fprintf(stderr, "failed to acquire swap chain image!\n");

  update_uniform_buffer(engine, imageIndex);

  VkSubmitInfo submitInfo = {0};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {vulkan_renderer->image_available_semaphores[vulkan_renderer->current_frame]};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &vulkan_renderer->command_buffers[imageIndex];

  VkSemaphore signalSemaphores[] = {vulkan_renderer->render_finished_semaphores[vulkan_renderer->current_frame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  vkResetFences(vulkan_renderer->device, 1, &vulkan_renderer->in_flight_fences[vulkan_renderer->current_frame]);

  result = vkQueueSubmit(vulkan_renderer->graphics_queue, 1, &submitInfo, vulkan_renderer->in_flight_fences[vulkan_renderer->current_frame]);

  if (result != VK_SUCCESS)
    fprintf(stderr, "Error to submit draw command buffer!\n");

  VkPresentInfoKHR presentInfo = {0};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {vulkan_renderer->swap_chain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  presentInfo.pImageIndices = &imageIndex;

  result = vkQueuePresentKHR(vulkan_renderer->present_queue, &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vulkan_renderer->framebuffer_resized) {
    vulkan_renderer->framebuffer_resized = false;
    recreate_swap_chain(vulkan_renderer);
  } else if (result != VK_SUCCESS)
    fprintf(stderr, "failed to present swap chain image!\n");

  vulkan_renderer->current_frame = (vulkan_renderer->current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void engine_update(struct Engine* engine) {
  engine->fps_counter.now_time = get_time();
  engine->fps_counter.delta_time += (engine->fps_counter.now_time - engine->fps_counter.last_time) / engine->fps_counter.limit_update_fps;
  engine->fps_counter.last_time = engine->fps_counter.now_time;

  while (engine->fps_counter.delta_time >= 1.0) {
    logic(engine, engine->fps_counter.delta_time / 20);
    engine->fps_counter.updates++;
    engine->fps_counter.delta_time--;
  }

  // Render frame
  render(engine);

  // glfwSwapBuffers(engine->window.glfw_window);
  glfwPollEvents();

  process_input(engine);

  // Will need to move input code to python
  if (engine->keys[GLFW_KEY_ESCAPE].state == PRESSED)
    glfwSetWindowShouldClose(engine->window.renderer.vulkan_renderer.glfw_window, true);

  engine->fps_counter.frames++;

  if (get_time() - engine->fps_counter.timer > 1.0) {
    engine->fps_counter.timer++;

    engine->fps_counter.second_target_fps = engine->fps_counter.target_fps;
    engine->fps_counter.second_average_fps = engine->fps_counter.average_fps;
    engine->fps_counter.second_frames = engine->fps_counter.frames;
    engine->fps_counter.second_updates = engine->fps_counter.updates;

    char title_buffer[200];
    sprintf(title_buffer, "Grindstone %d", (int)engine->fps_counter.second_frames);

    glfwSetWindowTitle(engine->window.renderer.vulkan_renderer.glfw_window, title_buffer);

    engine->fps_counter.fps = engine->fps_counter.frames;

    float averageCalc = 0;
    for (int loopNum = FPS_COUNT - 1; loopNum >= 0; loopNum--) {
      if (loopNum != 0)
        engine->fps_counter.fps_past[loopNum] = engine->fps_counter.fps_past[loopNum - 1];

      averageCalc += engine->fps_counter.fps_past[loopNum];
    }
    engine->fps_counter.fps_past[0] = engine->fps_counter.fps;
    engine->fps_counter.average_fps = averageCalc / FPS_COUNT;

    engine->fps_counter.updates = 0, engine->fps_counter.frames = 0;
  }
}

void logic(struct Engine* engine, double deltaTime) {}

double get_time() {
  struct timespec currentTime;
  timespec_get(&currentTime, TIME_UTC);

  return (double)currentTime.tv_sec + (double)currentTime.tv_nsec / 1000000000;
}

void process_input(struct Engine* engine) {
  for (int loopNum = 0; loopNum < KEY_LIMIT; loopNum++) {
    if (glfwGetKey(engine->window.renderer.vulkan_renderer.glfw_window, loopNum) == GLFW_PRESS) {
      engine->keys[loopNum].state = PRESSED;
      engine->keys[loopNum].held = true;
    } else if (glfwGetKey(engine->window.renderer.vulkan_renderer.glfw_window, loopNum) == GLFW_RELEASE) {
      engine->keys[loopNum].state = RELEASED;
      engine->keys[loopNum].held = false;
    }
  }
}

void update_uniform_buffer(struct Engine* engine, uint32_t currentImage) {
  struct Window* window = &engine->window;
  struct VulkanRenderer* vulkan_renderer = &engine->window.renderer.vulkan_renderer;

  double time = fmod(get_time(), M_PI * 2);

  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
    //vkResetCommandBuffer(vulkan_renderer->command_buffers[i], VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    command_buffer_start(&engine->window.renderer.vulkan_renderer, i);

    vkCmdBindPipeline(vulkan_renderer->command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_renderer->graphics_pipeline);

    for (int entity_num = 0; entity_num < array_list_size(&engine->entities); entity_num++) {
      struct Sprite* entity = ((struct Sprite*)array_list_get(&engine->entities, entity_num));

      VkBuffer vertexBuffers[] = {entity->vertex_buffer};
      VkDeviceSize offsets[] = {0};
      vkCmdBindVertexBuffers(vulkan_renderer->command_buffers[i], 0, 1, vertexBuffers, offsets);
      vkCmdBindIndexBuffer(vulkan_renderer->command_buffers[i], entity->index_buffer, 0, VK_INDEX_TYPE_UINT32);
      vkCmdBindDescriptorSets(vulkan_renderer->command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_renderer->pipeline_layout, 0, 1, &entity->descriptor_sets[i], 0, NULL);

      vkCmdDrawIndexed(vulkan_renderer->command_buffers[i], entity->image_mesh->indices->size, 1, 0, 0, 0);
      //vkCmdDrawIndexed(vulkan_renderer->commandBuffers[i], 12, 1, 0, 0, 0);
    }

    command_buffer_end(&engine->window.renderer.vulkan_renderer, i);
  }

  for (int entity_num = 0; entity_num < array_list_size(&engine->entities); entity_num++) {
    struct UniformBufferObject ubo = {{{0}}};

    ubo.model[0][0] = 1.0f;
    ubo.model[1][1] = 1.0f;
    ubo.model[2][2] = 1.0f;
    ubo.model[3][3] = 1.0f;

    glm_rotate(ubo.model, time, (vec3){0.0f, 0.0f + entity_num / 3.14159265358979, 1.0f});
    glm_lookat((vec3){2.0f, 2.0f, 2.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 0.0f, 1.0f}, ubo.view);
    glm_perspective(glm_rad(45.0f), (float)window->renderer.vulkan_renderer.swap_chain_extent.width / (float)window->renderer.vulkan_renderer.swap_chain_extent.height, 0.1f, 10.0f, ubo.proj);
    ubo.proj[1][1] *= -1;

    void* data;
    vkMapMemory(window->renderer.vulkan_renderer.device, ((struct Sprite*)array_list_get(&engine->entities, entity_num))->uniform_buffers_memory[currentImage], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(window->renderer.vulkan_renderer.device, ((struct Sprite*)array_list_get(&engine->entities, entity_num))->uniform_buffers_memory[currentImage]);
  }
}
