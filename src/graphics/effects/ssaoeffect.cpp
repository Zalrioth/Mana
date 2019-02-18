#include "graphics/effects/ssaoeffect.hpp"

//https://learnopengl.com/Advanced-Lighting/SSAO
//https://gamedev.stackexchange.com/questions/86621/how-can-i-pass-a-stdvectorvector3f-to-my-shader
//https://forum.facepunch.com/games/odkn/PSA-raise-your-SSAO-bias/1/

SSAOEffect::SSAOEffect(int width, int height)
{
    this->ssaoShader = new Shader("assets/shaders/ssao/ssao.vs", "assets/shaders/ssao/ssao.fs");
    this->ssaoBlurShader = new Shader("assets/shaders/ssao/ssao.vs", "assets/shaders/ssao/ssaoblur.fs");

    // MIGHT NOT NEED THIS HARDCODED IN SHADER
    glGenVertexArrays(1, &this->VAO);

    this->gSSAOFBO = createFBO();
    this->gSSAOTexture = createTexture16f(width, height);
    attachTextureNum(this->gSSAOFBO, this->gSSAOTexture, 0);

    this->gNoiseTexture = this->creatSSAONoiseTexture();

    for (unsigned int i = 0; i < 64; ++i) {
        glm::vec3 sample(generateFloat() * 2.0 - 1.0, generateFloat() * 2.0 - 1.0, generateFloat());
        sample = glm::normalize(sample);
        sample *= generateFloat();
        float scale = (float)i / 64.0;
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
}

SSAOEffect::~SSAOEffect()
{
    delete this->ssaoShader;
    delete this->ssaoBlurShader;
}

void SSAOEffect::render(GBuffer* gBuffer, PostProcess* postProcess)
{
    postProcess->stop();

    float bkColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, bkColor);

    glBindFramebuffer(GL_FRAMEBUFFER, this->gSSAOFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(bkColor[0], bkColor[1], bkColor[2], bkColor[3]);

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glDisable(GL_DEPTH_TEST);

    const GLenum buffers[]{ GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, buffers);

    // SSAO pass
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBuffer->gNormalTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gBuffer->gLinearDepthTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gBuffer->gPositionTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, this->gNoiseTexture);

    this->ssaoShader->use();
    this->ssaoShader->setInt("gNormals", 0);
    this->ssaoShader->setInt("gLinearDepth", 1);
    this->ssaoShader->setInt("gPosition", 2);
    this->ssaoShader->setInt("noiseTexture", 3);
    this->ssaoShader->setVec3a("samples", this->ssaoKernel);
    this->ssaoShader->setMat4("projMatrix", gBuffer->projectionMatrix);
    //this->ssaoShader->setMat4("invProjMatrix", gBuffer->invProjectionMatrix);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Blur pass
    postProcess->start();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postProcess->getTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->gSSAOTexture);

    this->ssaoBlurShader->use();
    this->ssaoBlurShader->setInt("colorTexture", 0);
    this->ssaoBlurShader->setInt("ssaoTexture", 1);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    postProcess->swapBuffer();
}

GLuint SSAOEffect::creatSSAONoiseTexture()
{
    std::vector<glm::vec3> ssaoNoise;
    for (int i = 0; i < 16; i++) {
        glm::vec3 noise(generateFloat() * 2.0 - 1.0, generateFloat() * 2.0 - 1.0, 0.0f);
        ssaoNoise.push_back(noise);
    }
    GLuint noiseTexture;
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
    return noiseTexture;
}

float SSAOEffect::lerp(float a, float b, float f)
{
    return a + f * (b - a);
}
