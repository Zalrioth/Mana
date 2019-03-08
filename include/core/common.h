#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vulkan/vulkan.h>

#include "datastructures/vector.h"

#define SMALL_BUFFER 128
#define LARGE_BUFFER 8192

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define EXPORT __declspec(dllexport)