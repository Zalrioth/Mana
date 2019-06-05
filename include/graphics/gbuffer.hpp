#pragma once
#ifndef GBUFFER_HPP_
#define GBUFFER_HPP_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "graphics/graphicsutils.hpp"
#include "graphics/shader.hpp"
#include "graphics/window.hpp"

class GBuffer {

private:
    GLuint VAO;

public:
    GBuffer(int width, int height);
    ~GBuffer();
    void start();
    void stop(Window* window, glm::mat4 projectionMatrix, glm::mat4 vietMatrix);
    void clear();

    GLuint gGBufferFBO;
    GLuint gColorTexture;
    GLuint gNormalTexture;
    GLuint gLinearDepthTexture;
    GLuint gPositionTexture;
    GLuint gDepthTexture;
    GLuint gLightScatterTexture;

    //GLuint gColorDepthDownsampleFBO;
    //GLuint gColorDownsampleTexture;
    //GLuint gDepthDownsampleTexture;

    Shader* positionDepthShader = nullptr;
    Shader* downsampleShader = nullptr;

    glm::mat4 projectionMatrix;
    glm::mat4 invProjectionMatrix;
    glm::mat4 viewMatrix;
    float centerDepth;
};

#endif // GBUFFER_HPP_