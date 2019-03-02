#pragma once
#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#define GLEW_STATIC
#include <GL/glew.h> // This must appear before freeglut.h
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

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

private:
    VkInstance instance;
};

#endif // WINDOW_HPP_