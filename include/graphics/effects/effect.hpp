#pragma once
#ifndef EFFECT_HPP_
#define EFFECT_HPP_

#include "graphics/shader.hpp"

class Effect {

protected:
    Shader* ourShader = nullptr;

public:
    Effect(){};
    virtual ~Effect() { delete ourShader; };
    virtual void render() {};
};

#endif // EFFECT_HPP_