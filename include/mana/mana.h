#pragma once
#ifndef MANA_H
#define MANA_H

#include "mana/core/engine.h"

struct Mana {
  struct Engine engine;
};

enum {
  MANA_SUCCESS = 0,
  MANA_ENGINE_ERROR,
  MANA_LAST_ERROR
};

int mana_init(struct Mana* mana, struct EngineSettings engine_settings);
void mana_cleanup(struct Mana* mana);

#endif  // MANA_H