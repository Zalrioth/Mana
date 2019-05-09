#pragma once
#ifndef SSAO_EFFECT_HPP_
#define SSAO_EFFECT_HPP_

#include <vector>

#include "graphics/effects/effect.hpp"
#include "graphics/gbuffer.hpp"
#include "graphics/postprocess.hpp"

class SSAOEffect : public Effect {

private:
    GLuint VAO;
    Shader* ssaoShader = nullptr;
    Shader* ssaoBlurShader = nullptr;

    std::vector<glm::vec3> ssaoKernel;
    GLuint createSSAONoiseTexture();
    float lerp(float a, float b, float f);

public:
    SSAOEffect(int width, int height);
    ~SSAOEffect();
    void render(GBuffer* gBuffer, PostProcess* postProcess);
    GLuint gSSAOFBO;
    GLuint gSSAOTexture;
    GLuint gNoiseTexture;
};

#endif // SSAO_EFFECT_HPP_