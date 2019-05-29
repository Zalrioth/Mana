#include "graphics/effects/lightscatteringeffect.hpp"

LightScatteringEffect::LightScatteringEffect()
{
    this->lightScatteringShader = new Shader("assets/shaders/lightscattering.vs", "assets/shaders/lightscattering.fs");

    glGenVertexArrays(1, &this->VAO);

    this->lightScatteringShader->use();
    //this->lightScatteringShader->setFloat("exposure", 0.00034f);
    this->lightScatteringShader->setFloat("exposure", 0.00068f);
    this->lightScatteringShader->setFloat("decay", 1.0f);
    this->lightScatteringShader->setFloat("density", 0.84f);
    this->lightScatteringShader->setFloat("weight", 5.65f);
    this->lightScatteringShader->setVec2("lightPositionOnScreen", 0.5f, 0.5f);
}

LightScatteringEffect::~LightScatteringEffect()
{
    delete this->lightScatteringShader;
}

void LightScatteringEffect::render(PostProcess* postProcess)
{
    postProcess->start();

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postProcess->getTexture());

    glDisable(GL_DEPTH_TEST);

    this->lightScatteringShader->use();
    glUniform1i(glGetUniformLocation(this->lightScatteringShader->ID, "uColorTexture"), 0);
    //this->lightScatteringShader->setInt("uColorTexture", 0);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    postProcess->swapBuffer();
}
