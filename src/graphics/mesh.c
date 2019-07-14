#include "graphics/mesh.h"

void mesh_init(struct Mesh* mesh)
{
    mesh->vertices = malloc(sizeof(struct Vector));
    memset(mesh->vertices, 0, sizeof(struct Vector));
    vector_init(mesh->vertices, sizeof(struct Vertex));

    mesh->indices = malloc(sizeof(struct Vector));
    memset(mesh->indices, 0, sizeof(struct Vector));
    vector_init(mesh->indices, sizeof(uint16_t));
}

void mesh_delete(struct Mesh* mesh)
{
    vector_free(mesh->vertices);
    vector_free(mesh->indices);
}

void assign_vertex(struct Vector* vector, float x, float y, float z, float r, float g, float b, float u, float v)
{
    struct Vertex vertex = { { 0 } };
    vertex.position[0] = x;
    vertex.position[1] = y;
    vertex.position[2] = z;

    vertex.normal[0] = r;
    vertex.normal[1] = g;
    vertex.normal[2] = b;

    vertex.texCoord[0] = u;
    vertex.texCoord[1] = v;

    vector_add(vector, &vertex);
}

void assign_indice(struct Vector* vector, uint32_t indice)
{
    vector_add(vector, &indice);
}