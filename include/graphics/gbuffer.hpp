#pragma once
#ifndef GBUFFER_HPP_
#define GBUFFER_HPP_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "graphics/graphicsutils.hpp"
#include "graphics/shader.hpp"

class GBuffer {

private:
    GLuint VAO;

public:
    GBuffer(int width, int height);
    ~GBuffer();
    void start();
    void stop(glm::mat4 projectionMatrix);
    void clear();
    GLuint gGBufferFBO;
    GLuint gColorTexture;
    GLuint gNormalTexture;
    GLuint gLinearDepthTexture;
    GLuint gPositionTexture;
    GLuint gDepthTexture;
    Shader* positionDepthShader = nullptr;
    glm::mat4 projectionMatrix;
    glm::mat4 invProjectionMatrix;
};

#endif // GBUFFER_HPP_