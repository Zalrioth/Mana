#pragma once
#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <stdlib.h>
#include <string>

#include "core/enginesettings.hpp"
#include "core/scene.hpp"
#include "graphics/graphicsutils.hpp"

class Engine {
private:
    Scene* scene = nullptr;
    EngineSettings* engineSettings = nullptr;

public:
    Engine(std::string title);
    ~Engine();
    void setScene(Scene* scene);
    void run();
    EngineSettings* getEngineSettings();
};

#endif // ENGINE_HPP_