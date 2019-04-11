#include "core/engine.h"

int init_engine(struct Engine* engine)
{
    if (!glfwInit())
        return GLFW_ERROR;

    if (!glfwVulkanSupported())
        return VULKAN_SUPPORT_ERROR;

    memset(engine->keys, RELEASED, sizeof(engine->keys));

    engine->fpsCounter.limitUpdateFPS = 1.0 / 60.0;
    // maybe cap fps at 144 for limit? 30, 60, 120, 144
    engine->fpsCounter.deltaTime = 0;
    engine->fpsCounter.nowTime = 0;
    engine->fpsCounter.frames = 0;
    engine->fpsCounter.updates = 0;
    engine->fpsCounter.lastRenderTime = 0;

    engine->fpsCounter.targetFps = glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;
    engine->fpsCounter.limitDrawFPS = 1.0 / engine->fpsCounter.targetFps;
    engine->fpsCounter.fps = (int)engine->fpsCounter.targetFps;
    engine->fpsCounter.lastTime = get_time();
    engine->fpsCounter.timer = engine->fpsCounter.lastTime;
    for (int loopNum = 0; loopNum < FPS_COUNT; loopNum++)
        engine->fpsCounter.fpsPast[loopNum] = engine->fpsCounter.fps;

    return 0;
}

void delete_engine(struct Engine* engine)
{
}

void render(struct Window* window)
{
    VkResult result = vkWaitForFences(window->device, 1, &window->inFlightFences[window->currentFrame], VK_TRUE, UINT64_MAX);
    //vkResetFences(window->device, 1, &window->inFlightFences[window->currentFrame]);

    uint32_t imageIndex;
    result = vkAcquireNextImageKHR(window->device, window->swapChain, UINT64_MAX, window->imageAvailableSemaphores[window->currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain(window);
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        fprintf(stderr, "failed to acquire swap chain image!\n");

    updateUniformBuffer(window, imageIndex);

    VkSubmitInfo submitInfo = { 0 };
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { window->imageAvailableSemaphores[window->currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &window->commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = { window->renderFinishedSemaphores[window->currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    //for (int i = window->imageIndices.total - 1; i >= 0; i--)
    //fprintf(stderr, "print out Num: %d\n", *(int*)vector_get(&window->imageIndices, i));

    //for (int i = window->imageVertices.total - 1; i >= 0; i--)
    //fprintf(stderr, "print out Struct: %f\n", ((struct Vertex*)vector_get(&window->imageVertices, i))->color[0]);

    vkResetFences(window->device, 1, &window->inFlightFences[window->currentFrame]);

    result = vkQueueSubmit(window->graphicsQueue, 1, &submitInfo, window->inFlightFences[window->currentFrame]);

    if (result != VK_SUCCESS)
        fprintf(stderr, "Error to submit draw command buffer!\n");
    //throw std::runtime_error("failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo = { 0 };
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { window->swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(window->presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->framebufferResized) {
        window->framebufferResized = false;
        recreateSwapChain(window);
    } else if (result != VK_SUCCESS)
        fprintf(stderr, "failed to present swap chain image!\n");

    window->currentFrame = (window->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

// TODO: Make this whole function in python?
void update_engine(struct Engine* engine)
{
    engine->fpsCounter.nowTime = get_time();
    engine->fpsCounter.deltaTime += (engine->fpsCounter.nowTime - engine->fpsCounter.lastTime) / engine->fpsCounter.limitUpdateFPS;
    engine->fpsCounter.lastTime = engine->fpsCounter.nowTime;

    while (engine->fpsCounter.deltaTime >= 1.0) {
        logic(engine, engine->fpsCounter.deltaTime / 20);
        engine->fpsCounter.updates++;
        engine->fpsCounter.deltaTime--;
    }

    // Render frame
    render(&engine->window);

    //glfwSwapBuffers(engine->window.glfwWindow);
    glfwPollEvents();

    process_input(engine);

    // Will need to move input code to python
    if (engine->keys[GLFW_KEY_ESCAPE].state == PRESSED)
        glfwSetWindowShouldClose(engine->window.glfwWindow, true);

    engine->fpsCounter.frames++;

    if (get_time() - engine->fpsCounter.timer > 1.0) {
        engine->fpsCounter.timer++;

        engine->fpsCounter.secondTargetFps = engine->fpsCounter.targetFps;
        engine->fpsCounter.secondAverageFps = engine->fpsCounter.averageFps;
        engine->fpsCounter.secondFrames = engine->fpsCounter.frames;
        engine->fpsCounter.secondUpdates = engine->fpsCounter.updates;

        engine->fpsCounter.fps = engine->fpsCounter.frames;

        float averageCalc = 0;
        for (int loopNum = FPS_COUNT - 1; loopNum >= 0; loopNum--) {
            if (loopNum != 0)
                engine->fpsCounter.fpsPast[loopNum] = engine->fpsCounter.fpsPast[loopNum - 1];

            averageCalc += engine->fpsCounter.fpsPast[loopNum];
        }
        engine->fpsCounter.fpsPast[0] = engine->fpsCounter.fps;
        engine->fpsCounter.averageFps = averageCalc / FPS_COUNT;

        engine->fpsCounter.updates = 0, engine->fpsCounter.frames = 0;
    }
}

void logic(struct Engine* engine, double deltaTime)
{
}

double get_time()
{
    struct timespec currentTime;
    timespec_get(&currentTime, TIME_UTC);

    return (double)currentTime.tv_sec + (double)currentTime.tv_nsec / 1000000000;
}

void process_input(struct Engine* engine)
{
    for (int loopNum = 0; loopNum < KEY_LIMIT; loopNum++) {
        if (glfwGetKey(engine->window.glfwWindow, loopNum) == GLFW_PRESS) {
            engine->keys[loopNum].state = PRESSED;
            engine->keys[loopNum].held = true;
        } else if (glfwGetKey(engine->window.glfwWindow, loopNum) == GLFW_RELEASE) {
            engine->keys[loopNum].state = RELEASED;
            engine->keys[loopNum].held = false;
        }
    }
}

void updateUniformBuffer(struct Window* window, uint32_t currentImage)
{
    double time = fmod(get_time(), M_PI * 2);

    struct UniformBufferObject ubo = { { { 0 } } };

    ubo.model[0][0] = 1.0f;
    ubo.model[1][1] = 1.0f;
    ubo.model[2][2] = 1.0f;
    ubo.model[3][3] = 1.0f;

    glm_rotate(ubo.model, time, (vec3){ 0.0f, 0.0f, 1.0f });
    glm_lookat((vec3){ 2.0f, 2.0f, 2.0f }, (vec3){ 0.0f, 0.0f, 0.0f }, (vec3){ 0.0f, 0.0f, 1.0f }, ubo.view);
    glm_perspective(glm_rad(45.0f), (float)window->swapChainExtent.width / (float)window->swapChainExtent.height, 0.1f, 10.0f, ubo.proj);
    ubo.proj[1][1] *= -1;

    void* data;
    vkMapMemory(window->device, window->uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(window->device, window->uniformBuffersMemory[currentImage]);
}
