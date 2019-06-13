#pragma once
#ifndef HBAO_EFFECT_HPP_
#define HBAO_EFFECT_HPP_

#include <vector>

#include "graphics/effects/effect.hpp"
#include "graphics/gbuffer.hpp"
#include "graphics/postprocess.hpp"

class HBAOEffect : public Effect {

#define KERNEL_SIZE 64

private:
    GLuint VAO;
    Shader* ssaoShader = nullptr;
    Shader* ssaoBlurShader = nullptr;

    std::vector<glm::vec3> ssaoKernel;
    GLuint createSSAONoiseTexture();
    float lerp(float a, float b, float f);

public:
    HBAOEffect(int width, int height);
    ~HBAOEffect();
    void render(GBuffer* gBuffer, PostProcess* postProcess);
    GLuint gSSAOFBO;
    GLuint gSSAOTexture;
    GLuint gNoiseTexture;
};

#endif // HBAO_EFFECT_HPP_