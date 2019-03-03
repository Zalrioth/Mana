#pragma once
#ifndef ENGINE_H_
#define ENGINE_H_

#include "datastructures/vector.h"

#include "core/common.h"
#include "core/window.h"

#define FPS_COUNT 10

struct FPSCounter {
    int fpsPast[FPS_COUNT];
    double limitUpdateFPS;
    // maybe cap fps at 144 for limit? 30, 60, 120, 144
    double targetFps;
    double limitDrawFPS;
    double averageFps;
    int input[128];
    double lastTime, timer;
    double deltaTime;
    int nowTime;
    int frames;
    int updates;
    double lastRenderTime;
    int fps;
};

struct Engine {
    struct Window window;
    struct FPSCounter fpsCounter;
    vector entities;
};

double get_time();
void init_engine(struct Engine* engine);
void delete_engine(struct Engine* engine);
void update_engine(struct Engine* engine);
void logic(double deltaTime);
//vector_init(&entities);

#endif // ENGINE_H_