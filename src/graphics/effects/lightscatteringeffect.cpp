#include "graphics/effects/lightscatteringeffect.hpp"

LightScatteringEffect::LightScatteringEffect()
{
    this->lightScatteringShader = new Shader("assets/shaders/lightscattering.vs", "assets/shaders/lightscattering.fs");

    glGenVertexArrays(1, &this->VAO);

    this->lightScatteringShader->use();
    this->lightScatteringShader->setFloat("exposure", 10.0f);
    this->lightScatteringShader->setFloat("decay", 1.0f);
    this->lightScatteringShader->setFloat("density", 1.0f);
    this->lightScatteringShader->setFloat("weight", 1.0f);
    this->lightScatteringShader->setVec2("lightPositionOnScreen", 0.5f, 0.5f);
}

LightScatteringEffect::~LightScatteringEffect()
{
    delete this->lightScatteringShader;
}

void LightScatteringEffect::render(PostProcess* postProcess)
{
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postProcess->getTexture());

    glDisable(GL_DEPTH_TEST);

    this->lightScatteringShader->use();
    this->lightScatteringShader->setInt("uColorTexture", 0);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    postProcess->swapBuffer();
}
