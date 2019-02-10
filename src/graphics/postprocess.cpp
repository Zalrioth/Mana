#include "graphics/postprocess.hpp"

PostProcess::PostProcess(int width, int height)
{
    this->gPostProcessFBO = createFBO();
    this->gPingTexture = createTexture(width, height);
    this->gPongTexture = createTexture(width, height);
    this->gSpareFloatTexture = createFloatingTexture(width, height);
    attachTextureNum(this->gPostProcessFBO, this->gPingTexture, 0);
    attachTextureNum(this->gPostProcessFBO, this->gPongTexture, 1);
    attachTextureNum(this->gPostProcessFBO, this->gSpareFloatTexture, 2);
}

PostProcess::~PostProcess()
{
    glDeleteFramebuffers(1, &this->gPostProcessFBO);
}

void PostProcess::swapTexture()
{
    if (this->pingPong) {
        attachTextureNum(this->gPostProcessFBO, this->gPongTexture, 0);
        attachTextureNum(this->gPostProcessFBO, this->gPingTexture, 1);
    } else {
        attachTextureNum(this->gPostProcessFBO, this->gPingTexture, 0);
        attachTextureNum(this->gPostProcessFBO, this->gPongTexture, 1);
    }

    this->pingPong ^= true;
}

GLuint PostProcess::getTexture()
{
    this->pingPong ^= true;

    return (this->pingPong) ? this->gPingTexture : this->gPongTexture;
}

GLuint PostProcess::getDrawTexture()
{
    return (this->pingPong) ? this->gPingTexture : this->gPongTexture;
}

void PostProcess::start()
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->gPostProcessFBO);

    const GLenum buffers[]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, buffers);
}

void PostProcess::stop()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
}

void PostProcess::clearAll()
{
    float bkColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, bkColor);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(bkColor[0], bkColor[1], bkColor[2], bkColor[3]);
}
