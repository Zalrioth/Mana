#pragma once
#ifndef YOUR_MOMS_H
#define YOUR_MOMS_H

#include "mana/core/engine.h"

struct Mana {
    struct Engine engine;
};

enum { YOUR_MOMS_ENGINE_ERROR = 0, YOUR_MOMS_SUCCESS = 1, YOUR_MOMS_WINDOW_ERROR = 2 };

int mana_init(struct Mana *mana);
int mana_new_window(struct Mana *mana, int width, int height);
void mana_close_window(struct Mana *mana);
void mana_cleanup(struct Mana *mana);

#endif // YOUR_MOMS_H
