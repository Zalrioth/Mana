#include "core/mana.h"

// C naming conventions
//https://stackoverflow.com/questions/1722112/what-are-the-most-common-naming-conventions-in-c
//https://stackoverflow.com/questions/4316314/pass-struct-by-reference-in-c
//https://stackoverflow.com/questions/252780/why-should-we-typedef-a-struct-so-often-in-c
// C Error handeling
//https://www.ibm.com/support/knowledgecenter/en/SSGMCP_5.2.0/com.ibm.cics.ts.applicationprogramming.doc/topics/dfhp3c00145.html

int init()
{
    int engineError = init_engine(&engine);
    switch (engineError) {
    default:
        break;
    case (1):
        printf("Error initializing GLFW!\n");
        return 1;
    case (2):
        printf("Vulkan support not found!\n");
        return 1;
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
    case (1):
        printf("Error creating GLFW window!\n");
        return 1;
    case (2):
        printf("Failed to create Vulkan instance!\n");
        return 1;
    case (3):
        printf("Failed to create window surface!\n");
        return 1;
    case (4):
        printf("Failed to find GPUs with Vulkan support!\n");
        return 1;
    case (5):
        printf("Failed to find a suitable GPU!\n");
        return 1;
    case (6):
        printf("Failed to create logical device!\n");
        return 1;
    case (7):
        printf("Failed to create swap chain!\n");
        return 1;
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
