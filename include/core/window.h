//https://www.codementor.io/a_hathon/building-and-using-dlls-in-c-d7rrd4caz
//http://www.syntaxbook.com/post/25995O2-how-to-measure-time-in-milliseconds-using-ansi-c

#pragma once
#ifndef WINDOW_H_
#define WINDOW_H_

#include "datastructures/vector.h"

#include "core/common.h"

#define FPS_COUNT 10

struct QueueFamilyIndices {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
};

struct Window {
    GLFWwindow* glfwWindow;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    struct QueueFamilyIndices indices;
    int width;
    int height;
};

int init_window(struct Window* gameWindow);
void delete_window(struct Window* gameWindow);
int create_glfw_window(struct Window* gameWindow, int width, int height);

#endif // WINDOW_H_