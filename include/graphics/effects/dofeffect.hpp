#pragma once
#ifndef DOF_EFFECT_HPP_
#define DOF_EFFECT_HPP_

#include "graphics/effects/effect.hpp"
#include "graphics/gbuffer.hpp"
#include "graphics/modelentity.hpp"
#include "graphics/postprocess.hpp"
#include "graphics/window.hpp"

class DOFEffect : public Effect {

private:
    GLuint VAO;
    Shader* dofShader = nullptr;
    float dofSeek = 0.0f;

public:
    DOFEffect(int width, int height, float zNear, float zFar);
    ~DOFEffect();
    void render(GBuffer* gBuffer, PostProcess* postProcess);
};

#endif // DOF_EFFECT_HPP_