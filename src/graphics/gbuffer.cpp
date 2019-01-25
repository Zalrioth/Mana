#include "graphics/gbuffer.hpp"

GBuffer::GBuffer(int width, int height)
{
    this->gGBufferFBO = createFBO();
    //glGenFramebuffers(1, &this->gGBufferFBO);
    this->gColorTexture = createTexture(width, height);
    this->gNormalTexture = createTexture(width, height);
    this->gLinearDepthTexture = createTexture(width, height);
    this->gPositionTexture = createFloatingTexture(width, height);
    this->gDepthTexture = createDepthTexture(width, height);
    //glBindFramebuffer(GL_FRAMEBUFFER, this->gGBufferFBO);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->gColorTexture, 0);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->gNormalTexture, 0);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, this->gLinearDepthTexture, 0);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, this->gPositionTexture, 0);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->gDepthTexture, 0);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    const GLenum buffers[]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, buffers);

    float bkColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, bkColor);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(bkColor[0], bkColor[1], bkColor[2], bkColor[3]);
}

void GBuffer::stop()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
}

void GBuffer::clear()
{
}
