#include "graphics/gbuffer.hpp"

GBuffer::GBuffer(int width, int height)
{
    this->gGBufferFBO = createFBO();
    this->gColorTexture = createTexture(width, height);
    this->gNormalTexture = createTexture(width, height);
    this->gLinearDepthTexture = createTexture(width, height);
    this->gPositionTexture = createFloatingTexture(width, height);
    this->gDepthTexture = createDepthTexture(width, height);
    attachTextureNum(this->gGBufferFBO, this->gColorTexture, 0);
    attachTextureNum(this->gGBufferFBO, this->gNormalTexture, 1);
    attachTextureNum(this->gGBufferFBO, this->gLinearDepthTexture, 2);
    attachTextureNum(this->gGBufferFBO, this->gPositionTexture, 3);
    attachDepthTexture(this->gGBufferFBO, this->gDepthTexture);
}

GBuffer::~GBuffer()
{
    glDeleteFramebuffers(1, &this->gGBufferFBO);
}

void GBuffer::start()
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->gGBufferFBO);

    const GLenum buffers[]{ GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, buffers);
}

void GBuffer::stop()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
}

void GBuffer::clear()
{
}
