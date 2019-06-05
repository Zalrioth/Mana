#include "graphics/effects/lensflareeffect.hpp"

LensFlareEffect::LensFlareEffect(int width, int height)
{
    this->lensFlareShader = new Shader("assets/shaders/lensflare/lensflare.vs", "assets/shaders/lensflare/lensflare.fs");
    this->lensFlareBlurShader = new Shader("assets/shaders/lensflare/lensflare.vs", "assets/shaders/lensflare/lensflareblur.fs");
    this->lensFlareCompositeeShader = new Shader("assets/shaders/lensflare/lensflare.vs", "assets/shaders/lensflare/lensflarecomposite.fs");

    glGenVertexArrays(1, &this->VAO);

    for (int loopNum = 0; loopNum <= 1; loopNum++) {
        this->gLensFlareFBO[loopNum] = createFBO();
        this->gLensFlareTexture[loopNum] = createTexture16f(width, height);
        attachTextureNum(this->gLensFlareFBO[loopNum], this->gLensFlareTexture[loopNum], 0);
    }

    //http://jeromeetienne.github.io/threex.sslensflare/examples/basic.html
    // 1 for threshold doesn't seem to do anything in this case

    this->lensFlareShader->use();
    this->lensFlareShader->setFloat("uDownsample", 2.0f);
    this->lensFlareShader->setInt("uGhostCount", 4); // 4, 0-32
    this->lensFlareShader->setFloat("uGhostSpacing", 0.4f); // 0.1f, 0.0f-2.0f
    this->lensFlareShader->setFloat("uGhostThreshold", 0.8f); // 2.0f, 0.0f-20.0f
    this->lensFlareShader->setFloat("uHaloRadius", 0.5f); // 0.6f, 0.0f-2.0f
    this->lensFlareShader->setFloat("uHaloThickness", 0.05f); // 0.1f, 0.0f-0.4f
    this->lensFlareShader->setFloat("uHaloThreshold", 0.8f); // 2.0f, 0.0f-20.0f
    this->lensFlareShader->setFloat("uHaloAspectRatio", 1.0f); // 1.0f, 0.0f-2.0f
    this->lensFlareShader->setFloat("uChromaticAberration", 0.01f); // 0.01f, 0.0f-0.2f

    //this->lensFlareBlurShader->use();
    //this->lensFlareBlurShader->setVec2("resolution", width, height);

    this->lensFlareCompositeeShader->use();
    this->lensFlareCompositeeShader->setFloat("uGlobalBrightness", 1.0f); // 0.01f, 0.0f-1.0f

    this->starburstTexture.id = TextureFromFile("starburst.png", "assets/textures");
    this->starburstTexture.type = "png";
    this->starburstTexture.path = "assets/textures/starburst.png";

    this->lensDirtTexture.id = TextureFromFile("lens_dirt.png", "assets/textures");
    this->lensDirtTexture.type = "png";
    this->lensDirtTexture.path = "assets/textures/lens_dirt.png";
}

LensFlareEffect::~LensFlareEffect()
{
}

void LensFlareEffect::render(Camera* camera, GBuffer* gBuffer, PostProcess* postProcess)
{
    postProcess->stop();

    float bkColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, bkColor);

    glBindFramebuffer(GL_FRAMEBUFFER, this->gLensFlareFBO[0]);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, this->gLensFlareFBO[1]);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glClearColor(bkColor[0], bkColor[1], bkColor[2], bkColor[3]);

    // Create lens flare pass
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glDisable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, this->gLensFlareFBO[0]);

    const GLenum buffers[]{ GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, buffers);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBuffer->gColorTexture);
    //glBindTexture(GL_TEXTURE_2D, postProcess->getTexture());
    this->lensFlareShader->use();
    this->lensFlareShader->setInt("txSceneColor", 0);
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Blur pass 1
    glBindFramebuffer(GL_FRAMEBUFFER, this->gLensFlareFBO[1]);
    glBindTexture(GL_TEXTURE_2D, this->gLensFlareTexture[0]);
    this->lensFlareBlurShader->use();
    this->lensFlareBlurShader->setInt("uColorTexture", 0);
    this->lensFlareBlurShader->setInt("direction", 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Blur pass 2
    glBindFramebuffer(GL_FRAMEBUFFER, this->gLensFlareFBO[0]);
    glBindTexture(GL_TEXTURE_2D, this->gLensFlareTexture[1]);
    this->lensFlareBlurShader->setInt("direction", 1);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Composite pass
    postProcess->start();

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postProcess->getTexture());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->gLensFlareTexture[0]);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->starburstTexture.id);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, this->lensDirtTexture.id);

    //float starburstOffset = camera->Position.x + camera->Position.y + camera->Position.z;
    float starburstOffset = camera->Front.x + camera->Front.y + camera->Front.z;

    glDisable(GL_DEPTH_TEST);

    this->lensFlareCompositeeShader->use();
    this->lensFlareCompositeeShader->setInt("uColorTexture", 0);
    this->lensFlareCompositeeShader->setInt("txFeatures", 1);
    this->lensFlareCompositeeShader->setInt("txLensDirt", 2);
    this->lensFlareCompositeeShader->setInt("txStarburst", 3);
    this->lensFlareCompositeeShader->setFloat("uStarburstOffset", starburstOffset);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    postProcess->swapBuffer();
}
