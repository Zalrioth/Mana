#pragma once
#ifndef INPUT_MANAGER_HPP_
#define INPUT_MANAGER_HPP_

//#include <GL/glew.h>  // This must appear before freeglut.h
#include "graphics/window.hpp"
#include <GLFW/glfw3.h>

enum KeyState { PRESSED,
    RELEASED };
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

#endif // INPUT_MANAGER_HPP_