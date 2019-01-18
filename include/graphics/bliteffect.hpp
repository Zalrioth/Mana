#pragma once
#ifndef BLIT_EFFECT_HPP_
#define BLIT_EFFECT_HPP_

#include "graphics/effect.hpp"

class BlitEffect : public Effect {

private:
    GLuint VAO;

public:
    BlitEffect();
    ~BlitEffect();
    void render(GLuint texture);
};

#endif // BLIT_EFFECT_HPP_