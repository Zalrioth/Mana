#pragma once
#ifndef SCENE_HPP_
#define SCENE_HPP_

#include "core/enginesettings.hpp"

class Scene {
 public:
  Scene(){};
  virtual ~Scene(){};
  virtual void update(EngineSettings *engineSettings, float deltaTime) = 0;
  virtual void render(EngineSettings *engineSettings) = 0;
};

#endif  // SCENE_HPP_