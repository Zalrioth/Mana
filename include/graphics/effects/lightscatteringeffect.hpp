#pragma once
#ifndef LIGHT_SCATTERING_EFFECT_HPP_
#define LIGHT_SCATTERING_EFFECT_HPP_

#include "graphics/effects/effect.hpp"
#include "graphics/gbuffer.hpp"
#include "graphics/modelentity.hpp"
#include "graphics/postprocess.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class LightScatteringEffect : public Effect {

private:
    GLuint VAO;
    Shader* lightScatteringShader = nullptr;

public:
    LightScatteringEffect();
    ~LightScatteringEffect();
    void render(Window* window, ModelEntity* lightSource, GBuffer* gBuffer, PostProcess* postProcess);
};

#endif // LIGHT_SCATTERING_EFFECT_HPP_