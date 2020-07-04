#pragma once
#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

// TODO: Include header that checks for platform
// TODO: Check with preprocessor if these can be moved outside or redefined easily

#define CHECK_MEMORY_LEAKS
#ifdef CHECK_MEMORY_LEAKS  // TODO: And windows
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
//
#include <crtdbg.h>
//
#include <string.h>
#else
#include <stdlib.h>
#include <string.h>
#endif

// Override c std lib memory allocator
#define CUSTOM_ALLOCATOR
#ifdef CUSTOM_ALLOCATOR
#include <mimalloc-override.h>
#endif

#endif  // MEMORY_ALLOCATOR_H
