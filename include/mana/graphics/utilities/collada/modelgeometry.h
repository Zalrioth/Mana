#pragma once
#ifndef MODEL_GEOMETRY_H
#define MODEL_GEOMETRY_H

#include "mana/core/memoryallocator.h"
//
#include <cglm/cglm.h>
#include <cstorage/cstorage.h>

#include "mana/core/corecommon.h"
#include "mana/core/xmlnode.h"

struct Mesh* geometry_loader_extract_model_data(struct XmlNode* geometry_node, struct Vector* vertex_weights);
void geomtry_leader_read_raw_data(struct XmlNode* mesh_data);
void geometry_loader_read_positions(struct XmlNode* mesh_data, struct Vector* vertices);
void geometry_loader_read_normals(struct XmlNode* mesh_data, struct Vector* normals);
void geometry_loader_read_texture_coordinates(struct XmlNode* mesh_data, struct Vector* tex_coords);

#endif  // MODEL_GEOMETRY_H
