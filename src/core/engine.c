#include "core/engine.h"

int init_engine(struct Engine* engine)
{
    if (!glfwInit())
        return 1;

    if (!glfwVulkanSupported())
        return 2;

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
    vkWaitForFences(window->device, 1, &window->inFlightFences[window->currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(window->device, 1, &window->inFlightFences[window->currentFrame]);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(window->device, window->swapChain, UINT64_MAX, window->imageAvailableSemaphores[window->currentFrame], VK_NULL_HANDLE, &imageIndex);

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

    if (vkQueueSubmit(window->graphicsQueue, 1, &submitInfo, window->inFlightFences[window->currentFrame]) != VK_SUCCESS)
        printf("Error to submit draw command buffer!\n");
    //throw std::runtime_error("failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo = { 0 };
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { window->swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(window->presentQueue, &presentInfo);

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

    engine->fpsCounter.frames++;

    if (get_time() - engine->fpsCounter.timer > 1.0) {
        engine->fpsCounter.timer++;

        printf("Target FPS: %lf\n", engine->fpsCounter.targetFps);
        printf("Average FPS: %lf\n", engine->fpsCounter.averageFps);
        printf("Draw FPS: %d\n", engine->fpsCounter.frames);
        printf("Update FPS: %d\n\n", engine->fpsCounter.updates);

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
    if (engine->keys[GLFW_KEY_ESCAPE].state == PRESSED)
        glfwSetWindowShouldClose(engine->window.glfwWindow, true);
    //int i;

    /*VECTOR_ADD(entities, "Bonjour");
    VECTOR_ADD(entities, "tout");
    VECTOR_ADD(entities, "le");
    VECTOR_ADD(entities, "monde");

    for (i = 0; i < VECTOR_TOTAL(entities); i++)
        printf("%s ", VECTOR_GET(entities, char*, i));
    printf("\n");

    VECTOR_DELETE(entities, 3);
    VECTOR_DELETE(entities, 2);
    VECTOR_DELETE(entities, 1);

    VECTOR_SET(entities, 0, "Hello");
    VECTOR_ADD(entities, "World");

    for (i = 0; i < VECTOR_TOTAL(entities); i++)
        printf("%s ", VECTOR_GET(entities, char*, i));
    printf("\n");

    printf("before vector size: %d\n", VECTOR_TOTAL(entities));

    for (i = VECTOR_TOTAL(entities) - 1; i >= 0; i--)
        VECTOR_DELETE(entities, i);

    printf("total vector size: %d\n", VECTOR_TOTAL(entities));

    //VECTOR_FREE(entities);*/

    //display();
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