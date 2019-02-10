#include "graphics/effects/ssaoeffect.hpp"

SSAOEffect::SSAOEffect()
{
    this->ssaoShader = new Shader("assets/shaders/ssao.vs", "assets/shaders/ssao.fs");
    this->ssaoBlurShader = new Shader("assets/shaders/ssaoblur.vs", "assets/shaders/ssaoblur.fs");

    // MIGHT NOT NEED THIS HARDCODED IN SHADER
    glGenVertexArrays(1, &this->VAO);
}

SSAOEffect::~SSAOEffect()
{
    delete this->ssaoShader;
    delete this->ssaoBlurShader;
}

void SSAOEffect::render(GBuffer* gBuffer, PostProcess* postProcess)
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