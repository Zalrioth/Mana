#include "graphics/effects/bloomeffect.hpp"

BloomEffect::BloomEffect()
{
    this->bloomShader = new Shader("assets/shaders/bloom/bloom.vs", "assets/shaders/bloom/bloom.fs");
    this->bloomThresholdShader = new Shader("assets/shaders/bloom/bloom.vs", "assets/shaders/bloom/bloomthresh.fs");
    this->bloomBlurShader = new Shader("assets/shaders/bloom/bloom.vs", "assets/shaders/bloom/bloomblur.fs");

    glGenVertexArrays(1, &this->VAO);
}

BloomEffect::~BloomEffect()
{
    delete this->bloomShader;
    delete this->bloomThresholdShader;
    delete this->bloomBlurShader;
}

void BloomEffect::render(GBuffer* gBuffer, PostProcess* postProcess)
{
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postProcess->getDrawTexture());

    glDisable(GL_DEPTH_TEST);

    this->ourShader->use();

    glUniform1i(glGetUniformLocation(this->ourShader->ID, "uColorTexture"), 0);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    postProcess->swapTexture();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}