//https://www.codementor.io/a_hathon/building-and-using-dlls-in-c-d7rrd4caz
//http://www.syntaxbook.com/post/25995O2-how-to-measure-time-in-milliseconds-using-ansi-c

#pragma once
#ifndef WINDOW_H_
#define WINDOW_H_

#include "datastructures/vector.h"

#include "core/common.h"

#define FPS_COUNT 10

struct Window {
    GLFWwindow* glfwWindow;
} window;

void init_window(struct Window* gameWindow);
void delete_window(struct Window* gameWindow);
void create_glfw_window(struct Window* gameWindow);

#endif // WINDOW_H_