#pragma once
#ifndef ENGINE_SETTINGS_HPP_
#define ENGINE_SETTINGS_HPP_

#include <GL/glew.h>  // This must appear before freeglut.h
#include <GLFW/glfw3.h>
//#include <btBulletDynamicsCommon.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>

#include "entity.hpp"
#include "graphics/camera.hpp"
#include "graphics/window.hpp"
#include "inputmanager.hpp"
#include "util.hpp"

class EngineSettings {
 public:
  std::string title;
  Window *window = nullptr;
  InputManager *inputManager = nullptr;
  Camera *camera = nullptr;
  // ShaderPrograms *shaderPrograms = nullptr;
  /*btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
  btCollisionDispatcher* dispatcher = nullptr;
  btBroadphaseInterface* overlappingPairCache = nullptr;
  btSequentialImpulseConstraintSolver* solver = nullptr;
  btDiscreteDynamicsWorld* dynamicsWorld = nullptr;*/
  glm::mat4 projectionMatrix;
  glm::mat4 viewMatrix;
  bool running;
  int fpsPast[10];
  float averageFps = 0.0;
  double xPosition = 0.0;
  double yPosition = 0.0;
  double yScroll = 0.0;
  EngineSettings(std::string title);
  virtual ~EngineSettings();
};

#endif  // ENGINE_SETTINGS_HPP_