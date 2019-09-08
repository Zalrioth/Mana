#include "graphics/model.h"

void modek_init(struct Model* model) {
}

int model_load(struct Model* model, char* directory, bool gamma) {
  const struct aiScene* scene = aiImportFile(directory, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

  if (!scene)
    return 1;

  model->gamma = gamma;

  model->root_matrix = scene->mRootNode->mTransformation;

  aiReleaseImport(scene);

  return NO_ERROR;
}

void process_node(struct aiNode* node, struct aiScene* scene) {
}