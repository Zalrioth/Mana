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
    void start();
    void stop();
    void clear();
    void swapBuffer();
    GLuint getBuffer();
    GLuint getTexture();
    GLuint gPostProcessFBO[2];
    GLuint gPingPongTexture[2];
    bool pingPong = false;
};

#endif // POSTPROCESS_HPP_