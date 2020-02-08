#pragma once
#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

#include <stdlib.h>
#include <string.h>

// Override c std lib memory allocator
#define CUSTOM_ALLOCATOR
#ifdef CUSTOM_ALLOCATOR
#include <mimalloc-override.h>
#endif

#endif  // MEMORY_ALLOCATOR_H
