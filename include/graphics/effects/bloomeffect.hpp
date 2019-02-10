#pragma once
#ifndef BLOOM_EFFECT_HPP_
#define BLOOM_EFFECT_HPP_

#include "graphics/effects/effect.hpp"
#include "graphics/gbuffer.hpp"
#include "graphics/postprocess.hpp"

class BloomEffect : public Effect {

private:
    GLuint VAO;
    Shader* bloomShader = nullptr;
    Shader* bloomThresholdShader = nullptr;
    Shader* bloomBlurShader = nullptr;

public:
    BloomEffect();
    ~BloomEffect();
    void render(GBuffer* gBuffer, PostProcess* postProcess);
};

#endif // BLOOM_EFFECT_HPP_