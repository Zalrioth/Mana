#pragma once
#ifndef MANA_H_
#define MANA_H_

#include "core/common.h"
#include "core/engine.h"

EXPORT bool init();
EXPORT bool new_window();
EXPORT void update();
EXPORT bool should_close();
EXPORT void close_window();
EXPORT void cleanup();

struct Engine engine;

#endif // MANA_H_