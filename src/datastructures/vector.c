#include "datastructures/vector.h"

void vector_init(struct Vector* v, int memorySize)
{
    v->memorySize = memorySize;
    v->capacity = VECTOR_INIT_CAPACITY;
    v->total = 0;
    v->items = malloc(sizeof(memorySize) * v->capacity);
}

int vector_total(struct Vector* v)
{
    return v->total;
}

void vector_resize(struct Vector* v, int capacity)
{
    void* items = realloc(v->items, sizeof(v->memorySize) * capacity);
    if (items) {
        free(v->items);
        v->items = items;
        v->capacity = capacity;
    }
}

void vector_add(struct Vector* v, void* item)
{
    if (v->capacity == v->total)
        vector_resize(v, v->capacity * 2);
    memcpy((char*)v->items + (v->memorySize * v->total), item, v->memorySize);
    v->total++;
}

void vector_set(struct Vector* v, int index, void* item)
{
    if (index >= 0 && index < v->total)
        memcpy((char*)v->items + (v->memorySize * index), item, v->memorySize);
}

void* vector_get(struct Vector* v, int index)
{
    if (index >= 0 && index < v->total)
        return (char*)v->items + (v->memorySize * index);
    return NULL;
}

void vector_delete(struct Vector* v, int index)
{
    if (index < 0 || index >= v->total)
        return;

    if (index != v->total - 1)
        memmove((char*)v->items + (v->memorySize * index), (char*)v->items + (v->memorySize * (index + 1)), v->memorySize);
    else
        memset((char*)v->items + (v->memorySize * index), 0, v->memorySize);

    v->total--;

    if (v->total > 0 && v->total == v->capacity / 4)
        vector_resize(v, v->capacity / 2);
}

void vector_free(struct Vector* v)
{
    free(v->items);
}

void vector_clear(struct Vector* v)
{
    for (int i = v->total - 1; i >= 0; i--)
        vector_delete(v, i);
}