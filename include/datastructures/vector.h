#pragma once
#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdio.h>
#include <stdlib.h>

#define VECTOR_INIT_CAPACITY 4

#define VECTOR_INIT(vec) \
    vector vec;          \
    vector_init(&vec)
#define VECTOR_ADD(vec, item) vector_add(&vec, (void*)item)
#define VECTOR_SET(vec, id, item) vector_set(&vec, id, (void*)item)
#define VECTOR_GET(vec, type, id) (type) vector_get(&vec, id)
#define VECTOR_DELETE(vec, id) vector_delete(&vec, id)
#define VECTOR_TOTAL(vec) vector_total(&vec)
#define VECTOR_FREE(vec) vector_free(&vec)

struct vector {
    void** items;
    int capacity;
    int total;
};

void vector_init(struct vector*);
int vector_total(struct vector*);
//static void vector_resize(struct vector*, int);
void vector_add(struct vector*, void*);
void vector_set(struct vector*, int, void*);
void* vector_get(struct vector*, int);
void vector_delete(struct vector*, int);
void vector_free(struct vector*);

#endif