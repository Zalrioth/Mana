#pragma once
#ifndef ENGINE_H_
#define ENGINE_H_

#include "core/common.h"
#include "core/window.h"

#define GLFW_ERROR 1
#define VULKAN_SUPPORT_ERROR 2

#define FPS_COUNT 10

#define KEY_LIMIT 512

enum KeyState { PRESSED,
    RELEASED };

struct Key {
    enum KeyState state;
    bool held;
};

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
    struct Key keys[KEY_LIMIT];
};

double get_time();
int init_engine(struct Engine* engine);
void delete_engine(struct Engine* engine);
void update_engine(struct Engine* engine);
void logic(struct Engine* engine, double deltaTime);
void process_input(struct Engine* engine);

#endif // ENGINE_H_