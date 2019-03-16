#include "core/mana.h"

// C naming conventions
//https://stackoverflow.com/questions/1722112/what-are-the-most-common-naming-conventions-in-c
//https://stackoverflow.com/questions/4316314/pass-struct-by-reference-in-c
//https://stackoverflow.com/questions/252780/why-should-we-typedef-a-struct-so-often-in-c
// C Error handeling
//https://www.ibm.com/support/knowledgecenter/en/SSGMCP_5.2.0/com.ibm.cics.ts.applicationprogramming.doc/topics/dfhp3c00145.html

void* readScript(void* vargp)
{
    printf("Printing GeeksQuiz from Thread \n");
    return NULL;
}

int init()
{
    int engineError = init_engine(&engine);
    switch (engineError) {
    default:
        break;
    case (GLFW_ERROR):
        printf("Error initializing GLFW!\n");
        return ENGINE_ERROR;
    case (VULKAN_SUPPORT_ERROR):
        printf("Vulkan support not found!\n");
        return ENGINE_ERROR;
    }

    //printf("Num of CPU: %d\n", omp_get_num_procs());

    /*omp_set_num_threads(4);

    #pragma omp parallel num_threads(4)
    {
        printf("thread %d\n", omp_get_thread_num());
    }*/

    /*omp_set_num_threads(omp_get_num_procs());

#pragma omp parallel for num_threads(4)
    for (int loopNum = 0; loopNum < 10; loopNum++) {
        printf("thread num %d\n", omp_get_thread_num());
        printf("hello openmp!\n");
    }*/

    /*int nthreads, tid;

#pragma omp parallel private(nthreads, tid)
    {
        tid = omp_get_thread_num();
        printf("Hello World from thread = %d\n", tid);

        if (tid == 0) {
            nthreads = omp_get_num_threads();
            printf("Number of threads = %d\n", nthreads);
        }
    }*/

    return 0;
}

int new_window(int width, int height)
{
    switch (init_window(&engine.window, width, height)) {
    default:
        break;
    case (CREATE_WINDOW_ERROR):
        printf("Error creating GLFW window!\n");
        return WINDOW_ERROR;
    case (CREATE_INSTANCE_ERROR):
        printf("Failed to create Vulkan instance!\n");
        return WINDOW_ERROR;
    case (SETUP_DEBUG_MESSENGER_ERROR):
        printf("Failed to set up debug messengerS!\n");
        return WINDOW_ERROR;
    case (CREATE_SURFACE_ERROR):
        printf("Failed to create window surface!\n");
        return WINDOW_ERROR;
    case (PICK_PHYSICAL_DEVICE_ERROR):
        printf("Failed to find a suitable GPU!\n");
        return WINDOW_ERROR;
    case (CREATE_LOGICAL_DEVICE_ERROR):
        printf("Failed to create logical device!\n");
        return WINDOW_ERROR;
    case (CREATE_SWAP_CHAIN_ERROR):
        printf("Failed to create swap chain!\n");
        return WINDOW_ERROR;
    case (CREATE_IMAGE_VIEWS_ERROR):
        printf("Failed to create image views!\n");
        return WINDOW_ERROR;
    case (CREATE_RENDER_PASS_ERROR):
        printf("Failed to create render pass!\n");
        return WINDOW_ERROR;
    case (CREATE_GRAPHICS_PIPELINE_ERROR):
        printf("Failed to create pipeline layout!\n");
        return WINDOW_ERROR;
    case (CREATE_FRAME_BUFFER_ERROR):
        printf("Failed to create framebuffer!\n");
        return WINDOW_ERROR;
    case (CREATE_COMMAND_POOL_ERROR):
        printf("Failed to create command pool!\n");
        return WINDOW_ERROR;
    case (CREATE_COMMAND_BUFFER_ERROR):
        printf("Failed to begin recording command buffer!\n");
        return WINDOW_ERROR;
    case (CREATE_SYNC_OBJECT_ERROR):
        printf("Failed to create synchronization objects for a frame!\n");
        return WINDOW_ERROR;
    }

    return NO_ERROR;
}

void update()
{
    update_engine(&engine);
}

bool should_close()
{
    if (glfwWindowShouldClose(engine.window.glfwWindow))
        return true;

    return false;
}

void close_window()
{
    delete_window(&engine.window);
}

void cleanup()
{
    delete_engine(&engine);
}

void print_fps()
{
    printf("Target FPS: %lf\n", engine.fpsCounter.secondTargetFps);
    printf("Average FPS: %lf\n", engine.fpsCounter.secondAverageFps);
    printf("Draw FPS: %d\n", engine.fpsCounter.secondFrames);
    printf("Update FPS: %d\n\n", engine.fpsCounter.secondUpdates);
}
