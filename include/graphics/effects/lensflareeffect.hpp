#pragma once
#ifndef LENS_FLARE_EFFECT_HPP_
#define LENS_FLARE_EFFECT_HPP_

#include "graphics/camera.hpp"
#include "graphics/effects/effect.hpp"
#include "graphics/gbuffer.hpp"
#include "graphics/postprocess.hpp"
#include "graphics/texture.hpp"

class LensFlareEffect : public Effect {

private:
    GLuint VAO;
    Shader* lensFlareShader = nullptr;
    Shader* lensFlareBlurShader = nullptr;
    Shader* lensFlareCompositeeShader = nullptr;
    GLuint gLensFlareFBO[2];
    GLuint gLensFlareTexture[2];

public:
    LensFlareEffect(int width, int height);
    ~LensFlareEffect();
    void render(Camera* camera, GBuffer* gBuffer, PostProcess* postProcess);
    Texture starburstTexture;
    Texture lensDirtTexture;
};

#endif // LENS_FLARE_EFFECT_HPP_