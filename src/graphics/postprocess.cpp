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
