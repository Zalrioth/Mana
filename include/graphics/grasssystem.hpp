#pragma once
#ifndef GRASS_SYSTEM_HPP_
#define GRASS_SYSTEM_HPP_

#include "core/enginesettings.hpp"
#include "graphics/transvoxelsystem.hpp"

class GrassSystem {

private:
    GLuint VAO;
    Shader* grassLODShader[3];

public:
    GrassSystem();
    ~GrassSystem();
    void render(EngineSettings* engineSettings, TransvoxelSystem* transvoxel);
};

#endif // GRASS_SYSTEM_HPP_