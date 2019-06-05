#pragma once
#ifndef VIGNETTE_EFFECT_HPP_
#define VIGNETTE_EFFECT_HPP_

#include "graphics/effects/effect.hpp"
#include "graphics/postprocess.hpp"
#include "graphics/window.hpp"

class VignetteEffect : public Effect {

private:
    GLuint VAO;
    Shader* vignetteShader = nullptr;

public:
    VignetteEffect(int width, int height);
    ~VignetteEffect();
    void render(PostProcess* postProcess);
};

#endif // VIGNETTE_EFFECT_HPP_