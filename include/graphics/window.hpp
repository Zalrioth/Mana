#pragma once
#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#define GLEW_STATIC
#include <GL/glew.h>  // This must appear before freeglut.h
#include <GLFW/glfw3.h>

#include <iostream>

class Window {
 public:
  Window(bool vSync, int width, int height);
  int setupWindow();
  ~Window();
  GLFWwindow* glWindow;
  bool vSync;
  bool limitFPS;
  int width;
  int height;
};

#endif  // WINDOW_HPP_