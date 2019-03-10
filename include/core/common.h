#define _CRT_SECURE_NO_DEPRECATE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vulkan/vulkan.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define IS_WINDOWS
#include <direct.h>
#else
#include <unistd.h>
#endif

#include "datastructures/vector.h"

#define ERROR !0
#define NO_ERROR 0

#define SMALL_BUFFER 128
#define LARGE_BUFFER 8192
#define HUGE_BUFFER 32768

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define EXPORT __declspec(dllexport)