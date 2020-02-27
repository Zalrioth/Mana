#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "mana/core/memoryallocator.h"
//
#include "mana/core/xmlparser.h"
#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/utilities/collada/modelgeometry.h"
#include "mana/graphics/utilities/collada/modelskeleton.h"
#include "mana/graphics/utilities/collada/modelskinning.h"
#include "mana/graphics/utilities/mesh.h"

struct Model {
  struct Texture* texture;
  struct SkeletonData* joints;
  struct Mesh* mesh;
};

void model_init(struct Model* model, char* path, int max_weights);

#endif  // MODEL_H
