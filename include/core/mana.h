#pragma once
#ifndef MANA_H_
#define MANA_H_

//#pragma clang diagnostic warning "-Wpragma-pack"

#include "core/common.h"
#include "core/engine.h"

#define ENGINE_ERROR 1
#define WINDOW_ERROR 2

int init();
int new_window(int width, int height);
void update();
bool should_close();
void close_window();
void cleanup();
void print_fps();

struct Engine engine;

#endif  // MANA_H_