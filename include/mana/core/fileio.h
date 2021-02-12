#pragma once
#ifndef FILE_IO_H
#define FILE_IO_H

#include "mana/core/memoryallocator.h"
//
#include "mana/core/corecommon.h"
#include "mana/graphics/render/vulkanrenderer.h"

// NOTE: Will only read binary file
static inline char* read_file(const char* filename, int* file_length) {
  FILE* fp = fopen(filename, "rb");
  char* result = NULL;

  // Could not open file
  if (fp == NULL)
    goto file_io_cleanup;

  fseek(fp, 0, SEEK_END);
  long int size = ftell(fp);
  rewind(fp);

  // Could not reach end of file
  if (size == -1)
    goto file_io_cleanup;

  *file_length = size;

  result = (char*)calloc(1, size + 1);

  // Could not read chunk of binary data
  if (fread(result, size, 1, fp) != 1) {
    free(result);
    result = NULL;
  }

  result[size] = '\0';

file_io_cleanup:
  fclose(fp);
  return result;
}

#endif  // FILE_IO_H
