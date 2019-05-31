#include "graphics/gbuffer.hpp"

GBuffer::GBuffer(int width, int height)
{
    this->gGBufferFBO = createFBO();
    this->gColorTexture = createTexture(width, height);
    this->gNormalTexture = createTexture(width, height);
    this->gLightScatterTexture = createTexture(width, height);
    this->gPositionTexture = createFloatingTexture(width, height);
    //this->gLinearDepthTexture = createTexture(width, height);
    this->gDepthTexture = createDepthTexture(width, height);

    attachTextureNum(this->gGBufferFBO, this->gColorTexture, 0);
    attachTextureNum(this->gGBufferFBO, this->gNormalTexture, 1);
    attachTextureNum(this->gGBufferFBO, this->gPositionTexture, 2);
    attachTextureNum(this->gGBufferFBO, this->gLightScatterTexture, 3);
    //attachTextureNum(this->gGBufferFBO, this->gLinearDepthTexture, 4);
    attachDepthTexture(this->gGBufferFBO, this->gDepthTexture);

    glGenVertexArrays(1, &this->VAO);

    this->positionDepthShader = new Shader("assets/shaders/positiondepth.vs", "assets/shaders/positiondepth.fs");
}

GBuffer::~GBuffer()
{
    glDeleteFramebuffers(1, &this->gGBufferFBO);
}

void GBuffer::start()
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->gGBufferFBO);

    const GLenum buffers[]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
    glDrawBuffers(5, buffers);
}

void GBuffer::stop(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->gDepthTexture);

    glDisable(GL_DEPTH_TEST);

    this->positionDepthShader->use();

    this->projectionMatrix = projectionMatrix;
    this->invProjectionMatrix = glm::inverse(projectionMatrix);
    this->viewMatrix = viewMatrix;
    this->positionDepthShader->setMat4("invProjMatrix", invProjectionMatrix);
    glUniform1i(glGetUniformLocation(this->positionDepthShader->ID, "gDepthTexture"), 0);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
}

void GBuffer::clear()
{
    float bkColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, bkColor);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(bkColor[0], bkColor[1], bkColor[2], bkColor[3]);
}
