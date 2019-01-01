#include "graphics/modelentity.hpp"

// https://github.com/assimp/assimp/issues/849
// https://lechior.blogspot.com/2017/05/skeletal-animation-using-assimp-opengl.html
// https://sourceforge.net/p/assimp/discussion/817653/thread/2f13d038/

ModelEntity::ModelEntity(const char* vertex, const char* fragment,
                         const char* model) {
  this->ourShader = new Shader(vertex, fragment);
  this->ourModel = new Model(model);
}

ModelEntity::~ModelEntity() {
  delete ourShader;
  delete ourModel;
}

void ModelEntity::setPosition(float x, float y, float z) {
  this->position->x = x;
  this->position->y = y;
  this->position->z = z;
}

void ModelEntity::setScale(float x, float y, float z) {
  this->scale->x = x;
  this->scale->y = y;
  this->scale->z = z;
}

inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from) {
  glm::mat4 to;
  // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
  to[0][0] = from.a1;
  to[1][0] = from.a2;
  to[2][0] = from.a3;
  to[3][0] = from.a4;
  to[0][1] = from.b1;
  to[1][1] = from.b2;
  to[2][1] = from.b3;
  to[3][1] = from.b4;
  to[0][2] = from.c1;
  to[1][2] = from.c2;
  to[2][2] = from.c3;
  to[3][2] = from.c4;
  to[0][3] = from.d1;
  to[1][3] = from.d2;
  to[2][3] = from.d3;
  to[3][3] = from.d4;
  return to;
}

void ModelEntity::render(EngineSettings* engineSettings) {
  this->ourShader->use();
  this->ourShader->setMat4("projection", engineSettings->projectionMatrix);
  this->ourShader->setMat4("view", engineSettings->viewMatrix);

  // glm::mat4 modelMatrix;
  glm::mat4 modelMatrix = aiMatrix4x4ToGlm(this->ourModel->rootMatrix);
  modelMatrix = glm::translate(modelMatrix, *this->position);
  modelMatrix = glm::scale(modelMatrix, *this->scale);
  this->ourShader->setMat4("model", modelMatrix);
  this->ourModel->Draw(*this->ourShader);
}

void ModelEntity::update(EngineSettings* engineSettings) {}
