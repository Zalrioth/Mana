#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "mana/core/memoryallocator.h"
//
#include "mana/core/xmlparser.h"
#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/utilities/mesh.h"

struct Model {
  //struct Texture texture;
  //struct Mesh mesh;
  bool animated;
};

void model_init(struct Model* model, char* path, bool animated);

#endif  // MODEL_H
