#pragma once
#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

// Override c std lib memory allocator
#if CHECK_MEMORY_LEAKS
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
//
#include <crtdbg.h>
//
#include <string.h>
// TODO: Add memory check for non windows
#else
#include <stdlib.h>
#include <string.h>
#endif
#else
#if CUSTOM_ALLOCATOR
#include <stdlib.h>
#include <string.h>
//
#include <mimalloc-override.h>
#else
#include <stdlib.h>
#include <string.h>
#endif
#endif

#endif  // MEMORY_ALLOCATOR_H
