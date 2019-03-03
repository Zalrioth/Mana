#include "core/engine.h"

int init_engine(struct Engine* engine)
{
    int windowError = init_window(&engine->window);
    switch (windowError) {
    default:
        break;
    case (1):
        printf("Error initializing GLFW!\n");
        return 1;
    case (2):
        printf("Vulkan support not found!\n");
        return 1;
    }

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

void update_engine(struct Engine* engine)
{
    engine->fpsCounter.nowTime = get_time();
    engine->fpsCounter.deltaTime += (engine->fpsCounter.nowTime - engine->fpsCounter.lastTime) / engine->fpsCounter.limitUpdateFPS;
    engine->fpsCounter.lastTime = engine->fpsCounter.nowTime;

    while (engine->fpsCounter.deltaTime >= 1.0) {
        logic(engine->fpsCounter.deltaTime / 20);
        engine->fpsCounter.updates++;
        engine->fpsCounter.deltaTime--;
    }

    glfwSwapBuffers(engine->window.glfwWindow);
    glfwPollEvents();

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

void logic(double deltaTime)
{
    int i;

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
