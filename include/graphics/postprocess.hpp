#pragma once
#ifndef POSTPROCESS_HPP_
#define POSTPROCESS_HPP_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "graphics/graphicsutils.hpp"

class PostProcess {

public:
    PostProcess(int width, int height);
    ~PostProcess();
    GLuint gPostProcessFBO;
    GLuint gPingTexture;
    GLuint gPongTexture;
    GLuint gSpareFloatTexture;
    bool pingPong = false;
};

#endif // POSTPROCESS_HPP_