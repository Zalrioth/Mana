#pragma once
#ifndef FILE_IO_H
#define FILE_IO_H

#include "mana/core/memoryallocator.h"
//
#include "mana/core/corecommon.h"
#include "mana/graphics/render/vulkanrenderer.h"

static inline char* read_file(const char* filename, int* file_length) {
  FILE* fp = fopen(filename, "rb");

  fseek(fp, 0, SEEK_END);
  long int size = ftell(fp);
  rewind(fp);

  *file_length = size;

  char* result = (char*)malloc(size);

  int index = 0;
  int c;
  while ((c = fgetc(fp)) != EOF) {
    result[index] = c;
    index++;
  }

  fclose(fp);

  return result;
}

#endif  // FILE_IO_H
