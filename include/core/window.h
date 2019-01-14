//https://www.codementor.io/a_hathon/building-and-using-dlls-in-c-d7rrd4caz
//http://www.syntaxbook.com/post/25995O2-how-to-measure-time-in-milliseconds-using-ansi-c

#pragma once
#ifndef WINDOW_INIT_H_
#define WINDOW_INIT_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "core/fpscount.h"
#include "datastructures/vector.h"
#include "graphics/shader.h"

#define EXPORT __declspec(dllexport)

EXPORT void new_window();
EXPORT bool should_close();
EXPORT void update();
EXPORT void close();

double get_time();
void logic(double deltaTime);

GLFWwindow* window;
vector entities;

#endif // WINDOW_INIT_H_