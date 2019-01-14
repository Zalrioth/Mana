#include "core/inputmanager.hpp"

// https://github.com/gnzlbg/glfw/blob/master/include/glfw/key.hpp
// http://www.glfw.org/docs/latest/group__keys.html#ga9845be48a745fc232045c9ec174d8820

InputManager::InputManager() {}

InputManager::~InputManager() {}

void InputManager::processInput(Window* window) {
  for (int loopNum = 0; loopNum < KEY_LIMIT; loopNum++) {
    if (glfwGetKey(window->glWindow, loopNum) == GLFW_PRESS) {
      this->keys[loopNum].state = PRESSED;
      this->keys[loopNum].held = true;
    } else if (glfwGetKey(window->glWindow, loopNum) == GLFW_RELEASE) {
      this->keys[loopNum].state = RELEASED;
      this->keys[loopNum].held = false;
    }
  }
}
