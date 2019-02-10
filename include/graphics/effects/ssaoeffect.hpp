#pragma once
#ifndef SSAO_EFFECT_HPP_
#define SSAO_EFFECT_HPP_

#include "graphics/effects/effect.hpp"
#include "graphics/gbuffer.hpp"
#include "graphics/postprocess.hpp"

class SSAOEffect : public Effect {

private:
    GLuint VAO;
    Shader* ssaoShader = nullptr;
    Shader* ssaoBlurShader = nullptr;

public:
    SSAOEffect();
    ~SSAOEffect();
    void render(GBuffer* gBuffer, PostProcess* postProcess);
};

#endif // SSAO_EFFECT_HPP_