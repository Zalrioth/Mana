#pragma once
#ifndef LIGHT_SCATTERING_EFFECT_HPP_
#define LIGHT_SCATTERING_EFFECT_HPP_

#include "graphics/effects/effect.hpp"
#include "graphics/gbuffer.hpp"
#include "graphics/postprocess.hpp"

class LightScatteringEffect : public Effect {

private:
    GLuint VAO;
    Shader* lightScatteringShader = nullptr;

public:
    LightScatteringEffect();
    ~LightScatteringEffect();
    void render(PostProcess* postProcess);
};

#endif // LIGHT_SCATTERING_EFFECT_HPP_