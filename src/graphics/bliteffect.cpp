#include "graphics/bliteffect.hpp"

BlitEffect::BlitEffect()
{
    this->ourShader = new Shader("assets/shaders/blit.vs", "assets/shaders/blit.fs");

    // MIGHT NOT NEED THIS HARDCODED IN SHADER
    glGenVertexArrays(1, &VAO);
}

BlitEffect::~BlitEffect()
{
}

void BlitEffect::render(GLuint texture)
{
    //glActiveTexture(GL_TEXTURE0);
    //glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    this->ourShader->use();

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}