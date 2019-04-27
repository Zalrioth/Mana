#pragma once
#ifndef EFFECT_HPP_
#define EFFECT_HPP_

#include "graphics/shader.hpp"

class Effect {

public:
    Effect(){};
    virtual ~Effect(){};
    virtual void render(){};
};

#endif // EFFECT_HPP_