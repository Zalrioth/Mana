#include "graphics/effects/dofeffect.hpp"

//https://www.popphoto.com/news/2012/06/how-to-calculate-f-stop-human-eye/
//https://threejs.org/examples/webgl_postprocessing_dof2.html

DOFEffect::DOFEffect(int width, int height)
{
    this->dofShader = new Shader("assets/shaders/dof.vs", "assets/shaders/dof.fs");

    glGenVertexArrays(1, &this->VAO);

    this->dofShader->use();
    this->dofShader->setInt("width", width);
    this->dofShader->setInt("height", height);
    this->dofShader->setFloat("focalLength", 12.0f); // 35.0 45.0
    this->dofShader->setFloat("fstop", 1.4f); // 2.2 8.0
}

DOFEffect::~DOFEffect()
{
    delete this->dofShader;
}

void DOFEffect::render(GBuffer* gBuffer, PostProcess* postProcess)
{
    postProcess->start();

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postProcess->getTexture());

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, gBuffer->gDepthTexture);

    glDisable(GL_DEPTH_TEST);

    //TODO: This will need to be changed to account for fps
    this->dofSeek += (gBuffer->centerDepth - this->dofSeek) * 0.15f;

    //std::cout << this->dofSeek << std::endl;

    this->dofShader->use();
    this->dofShader->setFloat("focalDepth", this->dofSeek); // 35.0 45.0
    this->dofShader->setInt("renderTex", 0);
    this->dofShader->setInt("depthTex", 1);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    postProcess->swapBuffer();
}
