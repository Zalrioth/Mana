#pragma once
#ifndef MANA_H
#define MANA_H

#include "core/common.h"
#include "core/engine.h"

#define ENGINE_ERROR 1
#define WINDOW_ERROR 2

struct Mana {
  struct Engine engine;
};

int mana_init(struct Mana* mana);
int mana_new_window(struct Mana* mana, int width, int height);
void mana_update(struct Mana* mana);
bool mana_should_close(struct Mana* mana);
void mana_close_window(struct Mana* mana);
void mana_cleanup(struct Mana* mana);
void mana_print_fps(struct Mana* mana);

#endif  // MANA_H