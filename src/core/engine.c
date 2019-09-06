#include "core/engine.h"

int engine_init(struct Engine* engine)
{
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

    return 0;
}

void engine_delete(struct Engine* engine) {}

void render(struct Window* window)
{
    VkResult result = vkWaitForFences(window->device, 1, &window->in_flight_fences[window->current_frame], VK_TRUE, UINT64_MAX);
    // vkResetFences(window->device, 1,
    // &window->inFlightFences[window->currentFrame]);

    uint32_t imageIndex;
    result = vkAcquireNextImageKHR(window->device, window->swap_chain, UINT64_MAX, window->image_available_semaphores[window->current_frame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swap_chain(window);
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        fprintf(stderr, "failed to acquire swap chain image!\n");

    update_uniform_buffer(window, imageIndex);

    VkSubmitInfo submitInfo = { 0 };
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { window->image_available_semaphores[window->current_frame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &window->command_buffers[imageIndex];

    VkSemaphore signalSemaphores[] = { window->render_finished_semaphores[window->current_frame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // for (int i = window->imageIndices.total - 1; i >= 0; i--)
    // fprintf(stderr, "print out Num: %d\n",
    // *(int*)vector_get(&window->imageIndices, i));

    // for (int i = window->imageVertices.total - 1; i >= 0; i--)
    // fprintf(stderr, "print out Struct: %f\n", ((struct
    // Vertex*)vector_get(&window->imageVertices, i))->color[0]);

    vkResetFences(window->device, 1, &window->in_flight_fences[window->current_frame]);

    result = vkQueueSubmit(window->graphics_queue, 1, &submitInfo, window->in_flight_fences[window->current_frame]);

    if (result != VK_SUCCESS)
        fprintf(stderr, "Error to submit draw command buffer!\n");
    // throw std::runtime_error("failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo = { 0 };
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { window->swap_chain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(window->present_queue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->framebuffer_resized) {
        window->framebuffer_resized = false;
        recreate_swap_chain(window);
    } else if (result != VK_SUCCESS)
        fprintf(stderr, "failed to present swap chain image!\n");

    window->current_frame = (window->current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

// TODO: Make this whole function in python?
void engine_update(struct Engine* engine)
{
    engine->fps_counter.now_time = get_time();
    engine->fps_counter.delta_time += (engine->fps_counter.now_time - engine->fps_counter.last_time) / engine->fps_counter.limit_update_fps;
    engine->fps_counter.last_time = engine->fps_counter.now_time;

    while (engine->fps_counter.delta_time >= 1.0) {
        logic(engine, engine->fps_counter.delta_time / 20);
        engine->fps_counter.updates++;
        engine->fps_counter.delta_time--;
    }

    // Render frame
    render(&engine->window);

    // glfwSwapBuffers(engine->window.glfw_window);
    glfwPollEvents();

    process_input(engine);

    // Will need to move input code to python
    if (engine->keys[GLFW_KEY_ESCAPE].state == PRESSED)
        glfwSetWindowShouldClose(engine->window.glfw_window, true);

    engine->fps_counter.frames++;

    if (get_time() - engine->fps_counter.timer > 1.0) {
        engine->fps_counter.timer++;

        engine->fps_counter.second_target_fps = engine->fps_counter.target_fps;
        engine->fps_counter.second_average_fps = engine->fps_counter.average_fps;
        engine->fps_counter.second_frames = engine->fps_counter.frames;
        engine->fps_counter.second_updates = engine->fps_counter.updates;

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

double get_time()
{
    struct timespec currentTime;
    timespec_get(&currentTime, TIME_UTC);

    return (double)currentTime.tv_sec + (double)currentTime.tv_nsec / 1000000000;
}

void process_input(struct Engine* engine)
{
    for (int loopNum = 0; loopNum < KEY_LIMIT; loopNum++) {
        if (glfwGetKey(engine->window.glfw_window, loopNum) == GLFW_PRESS) {
            engine->keys[loopNum].state = PRESSED;
            engine->keys[loopNum].held = true;
        } else if (glfwGetKey(engine->window.glfw_window, loopNum) == GLFW_RELEASE) {
            engine->keys[loopNum].state = RELEASED;
            engine->keys[loopNum].held = false;
        }
    }
}

void update_uniform_buffer(struct Window* window, uint32_t currentImage)
{
    double time = fmod(get_time(), M_PI * 2);

    struct UniformBufferObject ubo = { { { 0 } } };

    ubo.model[0][0] = 1.0f;
    ubo.model[1][1] = 1.0f;
    ubo.model[2][2] = 1.0f;
    ubo.model[3][3] = 1.0f;

    glm_rotate(ubo.model, time, (vec3){ 0.0f, 0.0f, 1.0f });
    glm_lookat((vec3){ 2.0f, 2.0f, 2.0f }, (vec3){ 0.0f, 0.0f, 0.0f }, (vec3){ 0.0f, 0.0f, 1.0f }, ubo.view);
    glm_perspective(glm_rad(45.0f), (float)window->swap_chain_extent.width / (float)window->swap_chain_extent.height, 0.1f, 10.0f, ubo.proj);
    ubo.proj[1][1] *= -1;

    void* data;
    vkMapMemory(window->device, window->uniform_buffers_memory[currentImage], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(window->device, window->uniform_buffers_memory[currentImage]);
}
