#include "graphics/effects/bliteffect.hpp"

BlitEffect::BlitEffect()
{
    this->blitShader = new Shader("assets/shaders/blit.vs", "assets/shaders/blit.fs");

    // MIGHT NOT NEED THIS HARDCODED IN SHADER
    glGenVertexArrays(1, &this->VAO);
}

BlitEffect::~BlitEffect()
{
}

void BlitEffect::render(GLuint texture)
{
    //glEnable(GL_TEXTURE_2D);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDisable(GL_DEPTH_TEST);

    this->blitShader->use();

    //glUniform1i(glGetUniformLocation(this->ourShader->ID, "uColorTexture"), 0);

    glUniform1i(glGetUniformLocation(this->blitShader->ID, "uColorTexture"), 0);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}