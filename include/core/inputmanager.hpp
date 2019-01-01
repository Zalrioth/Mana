#pragma once
#ifndef INPUT_MANAGER_HPP_
#define INPUT_MANAGER_HPP_

#include <GL/glew.h>  // This must appear before freeglut.h
#include <GLFW/glfw3.h>
#include "graphics/window.hpp"

enum KeyState { PRESSED, RELEASED };
const int KEY_LIMIT = 512;

struct Key {
  KeyState state = RELEASED;
  bool held = false;
};

class InputManager {
 public:
  InputManager();
  ~InputManager();
  void processInput(Window* window);
  Key keys[KEY_LIMIT];
};

#endif  // INPUT_MANAGER_HPP_