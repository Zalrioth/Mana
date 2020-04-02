#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "mana/core/memoryallocator.h"
//
#include "mana/core/xmlparser.h"
#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/render/vulkanrenderer.h"
#include "mana/graphics/utilities/collada/modelgeometry.h"
#include "mana/graphics/utilities/collada/modelskeleton.h"
#include "mana/graphics/utilities/collada/modelskinning.h"
#include "mana/graphics/utilities/mesh.h"

struct VulkanRenderer;

struct Model {
  struct SkeletonData* joints;
  struct Mesh* mesh;
  struct Texture* texture;
};

void model_init(struct Model* model, struct VulkanRenderer* vulkan_renderer, char* mode_path, char* texture_path, int max_weights);

#endif  // MODEL_H
