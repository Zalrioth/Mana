#include "graphics/effects/bloomeffect.hpp"

//https://learnopengl.com/Advanced-Lighting/Bloom

BloomEffect::BloomEffect(int width, int height)
{
    this->bloomShader = new Shader("assets/shaders/bloom/bloom.vs", "assets/shaders/bloom/bloom.fs");
    this->bloomThresholdShader = new Shader("assets/shaders/bloom/bloom.vs", "assets/shaders/bloom/bloomthresh.fs");
    this->bloomBlurShader = new Shader("assets/shaders/bloom/bloom.vs", "assets/shaders/bloom/bloomblur.fs");

    glGenVertexArrays(1, &this->VAO);

    for (int loopNum = 0; loopNum <= 1; loopNum++) {
        this->gBloomPingPongFBO[loopNum] = createFBO();
        this->gBloomPingPongTexture[loopNum] = createTexture16f(width, height);
        attachTextureNum(this->gBloomPingPongFBO[loopNum], this->gBloomPingPongTexture[loopNum], 0);
    }
}

BloomEffect::~BloomEffect()
{
    delete this->bloomShader;
    delete this->bloomThresholdShader;
    delete this->bloomBlurShader;

    glDeleteFramebuffers(2, this->gBloomPingPongFBO);
}

void BloomEffect::render(PostProcess* postProcess)
{
    postProcess->stop();

    float bkColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, bkColor);

    glBindFramebuffer(GL_FRAMEBUFFER, this->gBloomPingPongFBO[0]);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, this->gBloomPingPongFBO[1]);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glClearColor(bkColor[0], bkColor[1], bkColor[2], bkColor[3]);

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glDisable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, this->gBloomPingPongFBO[0]);

    const GLenum buffers[]{ GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, buffers);

    // Threshold pass
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postProcess->getTexture());
    this->bloomThresholdShader->use();
    this->bloomThresholdShader->setInt("uColorTexture", 0);
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Blur pass 1
    glBindFramebuffer(GL_FRAMEBUFFER, this->gBloomPingPongFBO[1]);
    glBindTexture(GL_TEXTURE_2D, this->gBloomPingPongTexture[0]);
    this->bloomBlurShader->use();
    this->bloomBlurShader->setInt("uColorTexture", 0);
    this->bloomBlurShader->setInt("direction", 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Blur pass 2
    glBindFramebuffer(GL_FRAMEBUFFER, this->gBloomPingPongFBO[0]);
    glBindTexture(GL_TEXTURE_2D, this->gBloomPingPongTexture[1]);
    this->bloomBlurShader->setInt("direction", 1);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Composite pass
    postProcess->start();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postProcess->getTexture());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->gBloomPingPongTexture[0]);

    this->bloomShader->use();
    this->bloomShader->setInt("uColorTexture", 0);
    this->bloomShader->setInt("uBloomTexture", 1);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    postProcess->swapBuffer();
}