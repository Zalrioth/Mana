#include "graphics/effects/fxaaeffect.hpp"

FXAAEffect::FXAAEffect(int width, int height)
{
    this->fxaaShader = new Shader("assets/shaders/fxaa.vs", "assets/shaders/fxaa.fs");

    glGenVertexArrays(1, &this->VAO);

    this->fxaaShader->use();
    this->fxaaShader->setVec2("uTexelStep", 1.0f / width, 1.0f / height);
    this->fxaaShader->setInt("uShowEdges", 0);
    this->fxaaShader->setInt("uFXAAOn", 1);
    this->fxaaShader->setFloat("uLumaThreshold", 0.5);
    this->fxaaShader->setFloat("uMulReduce", 8.0);
    this->fxaaShader->setFloat("uMinReduce", 128.0);
    this->fxaaShader->setFloat("uMaxSpan", 8.0);
}

FXAAEffect::~FXAAEffect()
{
    delete this->fxaaShader;
}

void FXAAEffect::render(PostProcess* postProcess)
{
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postProcess->getTexture());

    glDisable(GL_DEPTH_TEST);

    this->fxaaShader->use();
    this->fxaaShader->setInt("uColorTexture", 0);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    postProcess->swapBuffer();
}