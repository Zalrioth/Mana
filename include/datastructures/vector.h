#pragma once
#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdio.h>
#include <stdlib.h>

#define VECTOR_INIT_CAPACITY 4

struct Vector {
    void** items;
    int capacity;
    int total;
};

void vector_init(struct Vector* v);
int vector_total(struct Vector* v);
void vector_add(struct Vector* v, void* item);
void vector_set(struct Vector* v, int index, void* item);
void* vector_get(struct Vector* v, int index);
void vector_delete(struct Vector* v, int index);
void vector_free(struct Vector* v);
void vector_clear(struct Vector* v);

#endif