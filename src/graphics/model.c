#include "graphics/model.h"

void modek_init(struct Model* model) {
}

int model_load(struct Model* model, char* directory, bool gamma) {
  const struct aiScene* scene = aiImportFile(directory, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

  if (!scene)
    return 1;

  model->gamma = gamma;
  model->root_matrix = scene->mRootNode->mTransformation;

  model_process_node(model, scene->mRootNode, scene);

  aiReleaseImport(scene);

  return NO_ERROR;
}

void model_process_node(struct Model* model, struct aiNode* node, struct aiScene* scene) {
  for (unsigned int mesh_num = 0; mesh_num < node->mNumMeshes; mesh_num++) {
    struct aiMesh* mesh = scene->mMeshes[node->mMeshes[mesh_num]];
    vector_push_back(model->meshes, model_precess_mesh(mesh, scene));
  }

  for (unsigned int child_num = 0; child_num < node->mNumChildren; child_num++)
    model_process_node(model, node->mChildren[child_num], scene);
}

struct Mesh* model_process_mesh(struct Model* model, struct aiMesh* mesh, struct aiScene* scene) {
  struct Mesh* new_mesh = calloc(1, sizeof(struct Mesh));
  mesh_init(new_mesh);

  for (unsigned int vertice_num = 0; vertice_num < mesh->mNumVertices; vertice_num++) {
    if (mesh->mTextureCoords[0])
      mesh_assign_vertex_full(new_mesh->vertices, mesh->mVertices[vertice_num].x, mesh->mVertices[vertice_num].y, mesh->mVertices[vertice_num].z,
                              mesh->mNormals[vertice_num].x, mesh->mNormals[vertice_num].y, mesh->mNormals[vertice_num].z,
                              mesh->mTextureCoords[0][vertice_num].x, mesh->mTextureCoords[0][vertice_num].y,
                              mesh->mTangents[vertice_num].x, mesh->mTangents[vertice_num].y, mesh->mTangents[vertice_num].z,
                              mesh->mBitangents[vertice_num].x, mesh->mBitangents[vertice_num].y, mesh->mBitangents[vertice_num].z);
    else
      mesh_assign_vertex_full(new_mesh->vertices, mesh->mVertices[vertice_num].x, mesh->mVertices[vertice_num].y, mesh->mVertices[vertice_num].z,
                              mesh->mNormals[vertice_num].x, mesh->mNormals[vertice_num].y, mesh->mNormals[vertice_num].z,
                              0.0f, 0.0f,
                              mesh->mTangents[vertice_num].x, mesh->mTangents[vertice_num].y, mesh->mTangents[vertice_num].z,
                              mesh->mBitangents[vertice_num].x, mesh->mBitangents[vertice_num].y, mesh->mBitangents[vertice_num].z);
  }

  for (unsigned int face_num = 0; face_num < mesh->mNumFaces; face_num++) {
    struct aiFace face = mesh->mFaces[face_num];
    for (unsigned int indice_num = 0; indice_num < face.mNumIndices; indice_num++)
      mesh_assign_indice(new_mesh->indices, face.mIndices[indice_num]);
  }

  struct aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

  model_load_material_texture(new_mesh->textures, material, aiTextureType_DIFFUSE, "texture_diffuse");
  model_load_material_texture(new_mesh->textures, material, aiTextureType_SPECULAR, "texture_specular");
  model_load_material_texture(new_mesh->textures, material, aiTextureType_HEIGHT, "texture_normal");
  model_load_material_texture(new_mesh->textures, material, aiTextureType_AMBIENT, "texture_height");

  return new_mesh;
}

model_load_material_texture(struct Vector* vector, struct aiMaterial* mat, enum aiTextureType type, char* path) {
  for (unsigned int texture_num = 0; texture_num < mat->GetTextureCount(type)
}