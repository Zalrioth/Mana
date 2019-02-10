#include "graphics/postprocess.hpp"

PostProcess::PostProcess(int width, int height)
{
    this->gPostProcessFBO = createFBO();
    this->gPingTexture = createTexture(width, height);
    this->gPongTexture = createTexture(width, height);
    attachTextureNum(this->gPostProcessFBO, this->gPingTexture, 0);
    attachTextureNum(this->gPostProcessFBO, this->gPongTexture, 1);
}

PostProcess::~PostProcess()
{
    glDeleteFramebuffers(1, &this->gPostProcessFBO);
}

void PostProcess::start()
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->gPostProcessFBO);

    const GLenum buffers[]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, buffers);
}

void PostProcess::stop()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
}

void PostProcess::clear()
{
    float bkColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, bkColor);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(bkColor[0], bkColor[1], bkColor[2], bkColor[3]);

    this->pingPong = false;
}

void PostProcess::swapTexture()
{
    if (this->pingPong) {
        attachTextureNum(this->gPostProcessFBO, this->gPingTexture, 1);
        attachTextureNum(this->gPostProcessFBO, this->gPongTexture, 0);
    } else {
        attachTextureNum(this->gPostProcessFBO, this->gPingTexture, 0);
        attachTextureNum(this->gPostProcessFBO, this->gPongTexture, 1);
    }

    this->pingPong ^= true;
}

GLuint PostProcess::getTexture()
{
    return (this->pingPong) ? this->gPingTexture : this->gPongTexture;
}
