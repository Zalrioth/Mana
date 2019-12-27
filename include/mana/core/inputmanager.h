#pragma once
#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <GLFW/glfw3.h>
#include "mana/core/window.h"

enum KeyState { PRESSED,
                RELEASED };

struct Key {
  enum KeyState state;
  bool held;
};

#define KEY_LIMIT 512

struct InputManager {
  struct Key keys[KEY_LIMIT];
  double last_mouse_x_pos;
  double last_mouse_y_pos;
};

void input_manager_init(struct InputManager* input_manager);
void input_manager_process_input(struct InputManager* input_manager, struct Window* window);

#endif  // INPUT_MANAGER_H