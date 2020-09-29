#pragma once
#ifndef FILE_IO_H
#define FILE_IO_H

#include "mana/core/memoryallocator.h"
//
#include "mana/core/corecommon.h"
#include "mana/graphics/render/vulkanrenderer.h"

static inline char* read_file(const char* filename, int* file_length) {
  FILE* fp = fopen(filename, "rb");

  // Could not open file
  if (fp == NULL)
    return NULL;

  fseek(fp, 0, SEEK_END);
  long int size = ftell(fp);
  rewind(fp);

  // Could not reach end of file
  if (size == -1)
    return NULL;

  *file_length = size;

  char* result = (char*)malloc(size);

  int index = 0;
  int c;
  while ((c = getc(fp)) != EOF) {
    // Guard against writing over buffer
    if (index == size) {
      result[size - 1] = '\0';
      break;
    }

    result[index] = c;
    index++;
  }

  fclose(fp);

  return result;
}

#endif  // FILE_IO_H
