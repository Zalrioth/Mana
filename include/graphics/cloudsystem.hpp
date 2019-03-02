#pragma once
#ifndef CLOUD_SYSTEM_HPP_
#define CLOUD_SYSTEM_HPP_

#include "core/enginesettings.hpp"
#include <glm/glm.hpp>
#include <noise/module/modulebase.h>
#include <noise/noise.h>

using namespace noise;

class CloudSystem {

private:
GLuint VAO;
    GLuint createPerlinTexture();
    GLuint createWorleyTexture();
    GLuint createWeatherTexture();
    GLuint gCloudFBO;
    GLuint gCopyFrameFBO;
    Shader* volumetricCloudShader;
    Shader* cloudPostShader;
    Shader* copyFrameShader;
    float coverage;
    int frameIter = 0;
    glm::mat4 oldFrameVP;

public:
    CloudSystem(int width, int height);
    ~CloudSystem();
    void render(EngineSettings* engineSettings);
    GLuint gCloudTextures[3];
    GLuint gCopyFrameTexture[2];
    GLuint perlinTex, worley32, weatherTex;
};

#endif // CLOUD_SYSTEM_HPP_