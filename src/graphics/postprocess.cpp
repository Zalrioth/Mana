#include "graphics/postprocess.hpp"

PostProcess::PostProcess(int width, int height)
{
    //this->gPostProcessFBO = createFBO();
    glGenFramebuffers(1, &this->gPostProcessFBO);
    this->gPingTexture = createTexture(width, height);
    this->gPongTexture = createTexture(width, height);
    this->gSpareFloatTexture = createFloatingTexture(width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->gPingTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->gPongTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->gSpareFloatTexture, 0);
    //attachTextureNum(this->gPostProcessFBO, this->gPingTexture, 0);
    //attachTextureNum(this->gPostProcessFBO, this->gPongTexture, 1);
    //attachTextureNum(this->gPostProcessFBO, this->gSpareFloatTexture, 2);
}

PostProcess::~PostProcess()
{
    glDeleteFramebuffers(1, &this->gPostProcessFBO);
}
