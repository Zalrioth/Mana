#pragma once
#ifndef MESH_H_
#define MESH_H_

#include "core/common.h"
#include "graphics/texture.h"
#include "vector.h"
#include <cglm/cglm.h>

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
    vec3 tangent;
    vec3 bitTangent;
};

struct Mesh {
    struct Vector* vertices;
    struct Vector* indices;
    struct Vector* textures;
};

void mesh_init(struct Mesh* mesh);
void mesh_delete(struct Mesh* mesh);
void assign_vertex(struct Vector* vector, float x, float y, float z, float r, float g, float b, float u, float v);
void assign_indice(struct Vector* vector, uint32_t indice);

#endif // MESH_H_
