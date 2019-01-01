#include "core/enginesettings.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "graphics/stb_image.h"

EngineSettings::EngineSettings(std::string title) {
  this->title = title;
  this->running = true;
  this->inputManager = new InputManager();
  /*this->collisionConfiguration = new btDefaultCollisionConfiguration();
  this->dispatcher = new btCollisionDispatcher(this->collisionConfiguration);
  this->overlappingPairCache = new btDbvtBroadphase();
  this->solver = new btSequentialImpulseConstraintSolver;
  this->dynamicsWorld = new btDiscreteDynamicsWorld(this->dispatcher,
  this->overlappingPairCache, this->solver, this->collisionConfiguration);*/
  this->camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
}

EngineSettings::~EngineSettings() {
  delete this->window;
  delete this->inputManager;
  /*delete this->collisionConfiguration;
  delete this->dispatcher;
  delete this->overlappingPairCache;
  delete this->solver;
  delete this->dynamicsWorld;*/
}