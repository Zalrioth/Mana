#pragma once
#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <string.h>

// NOTE: Anything included after this will use new allocator
#include <mimalloc-override.h>

#define _CRT_SECURE_NO_DEPRECATE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
//#include <chaos/chaos.h>
#include <cstorage/cstorage.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <vulkan/vulkan.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define IS_WINDOWS
#include <direct.h>
#else
#include <unistd.h>
#endif

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define MAX_FRAMES_IN_FLIGHT 2
#define MAX_SWAP_CHAIN_FRAMES MAX_FRAMES_IN_FLIGHT + 1

#endif  // COMMON_H
