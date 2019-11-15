#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "core/common.h"
#include "graphics/mesh.h"
#include "graphics/modeldata.h"

struct Model {
  struct Texture texture;
  struct Mesh mesh;
  bool animated;
};

model_init(char* path, bool animated) {
}

#endif  // MODEL_H
