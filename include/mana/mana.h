#pragma once
#ifndef MANA_H
#define MANA_H

#include "mana/core/engine.h"

struct Mana {
  struct Engine engine;
};

enum {
  MANA_ENGINE_ERROR = 0,
  MANA_SUCCESS = 1,
  MANA_WINDOW_ERROR = 2
};

int mana_init(struct Mana* mana);
int mana_new_window(struct Mana* mana, int width, int height);
void mana_close_window(struct Mana* mana);
void mana_cleanup(struct Mana* mana);

#endif  // MANA_H