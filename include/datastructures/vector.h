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

void vector_init(struct Vector*);
int vector_total(struct Vector*);
void vector_add(struct Vector*, void*);
void vector_set(struct Vector*, int, void*);
void* vector_get(struct Vector*, int);
void vector_delete(struct Vector*, int);
void vector_free(struct Vector*);

#endif