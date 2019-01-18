#pragma once
#ifndef GBUFFER_HPP_
#define GBUFFER_HPP_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "graphics/graphicsutils.hpp"

class GBuffer {

public:
    GBuffer(int width, int height);
    ~GBuffer();
    void start();
    void stop();
    void clear();
    GLuint gGBufferFBO;
    GLuint gColorTexture;
    GLuint gNormalTexture;
    GLuint gLinearDepthTexture;
    GLuint gPositionTexture;
    GLuint gDepthTexture;
};

#endif // GBUFFER_HPP_