#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vulkan/vulkan.h>

#define SMALL_BUFFER 128

#define EXPORT __declspec(dllexport)