#include "graphics/effects/vignetteeffect.hpp"

VignetteEffect::VignetteEffect(int width, int height)
{
    this->vignetteShader = new Shader("assets/shaders/vignette.vs", "assets/shaders/vignette.fs");

    glGenVertexArrays(1, &this->VAO);

    this->vignetteShader->use();
    this->vignetteShader->setVec2("resolution", width, height);
}

VignetteEffect::~VignetteEffect()
{
    delete this->vignetteShader;
}

void VignetteEffect::render(PostProcess* postProcess)
{
    postProcess->start();

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postProcess->getTexture());

    glDisable(GL_DEPTH_TEST);

    this->vignetteShader->use();
    this->vignetteShader->setInt("uColorTexture", 0);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    postProcess->swapBuffer();
}
