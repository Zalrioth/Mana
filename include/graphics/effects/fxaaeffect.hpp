#pragma once
#ifndef FXAA_EFFECT_HPP_
#define FXAA_EFFECT_HPP_

#include "graphics/effects/effect.hpp"
#include "graphics/gbuffer.hpp"
#include "graphics/postprocess.hpp"

class FXAAEffect : public Effect {

private:
    GLuint VAO;
    Shader* fxaaShader = nullptr;

public:
    FXAAEffect(int width, int height);
    ~FXAAEffect();
    void render(PostProcess* postProcess);
};

#endif // FXAA_EFFECT_HPP_