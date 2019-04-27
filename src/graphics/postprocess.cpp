#include "graphics/postprocess.hpp"

PostProcess::PostProcess(int width, int height)
{
    for (int loopNum = 0; loopNum < 2; loopNum++) {
        this->gPostProcessFBO[loopNum] = createFBO();
        this->gPingPongTexture[loopNum] = createTexture(width, height);
        attachTextureNum(this->gPostProcessFBO[loopNum], this->gPingPongTexture[loopNum], 0);
    }
}

PostProcess::~PostProcess()
{
    glDeleteFramebuffers(2, this->gPostProcessFBO);
}

void PostProcess::start()
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->getBuffer());

    const GLenum buffers[]{ GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, buffers);
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

    glBindFramebuffer(GL_FRAMEBUFFER, this->gPostProcessFBO[1]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, this->gPostProcessFBO[0]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glClearColor(bkColor[0], bkColor[1], bkColor[2], bkColor[3]);

    this->pingPong = false;
}

void PostProcess::swapBuffer()
{
    this->pingPong ^= true;
}

GLuint PostProcess::getBuffer()
{
    return this->gPostProcessFBO[this->pingPong];
}

GLuint PostProcess::getTexture()
{
    return this->gPingPongTexture[!this->pingPong];
}
