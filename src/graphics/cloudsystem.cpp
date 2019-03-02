#include "graphics/cloudsystem.hpp"

CloudSystem::CloudSystem(int width, int height)
{
    this->volumetricCloudShader = new Shader("assets/shaders/screen.vs", "assets/shaders/clouds/volumetricclouds.fs");
    //this->cloudPostShader = new Shader("shaders/screen.vs", "shaders/clouds/cloudspost.fs");
    this->copyFrameShader = new Shader("assets/shaders/screen.vs", "assets/shaders/clouds/copyframe.fs");

    glGenVertexArrays(1, &this->VAO);

    this->gCloudFBO = createFBO();
    this->gCloudTextures[0] = createTexture(width, height);
    this->gCloudTextures[1] = createTexture(width, height);
    this->gCloudTextures[2] = createTexture(width, height);
    attachTextureNum(this->gCloudFBO, this->gCloudTextures[0], 0);
    attachTextureNum(this->gCloudFBO, this->gCloudTextures[1], 1);
    attachTextureNum(this->gCloudFBO, this->gCloudTextures[2], 2);

    this->gCopyFrameFBO = createFBO();
    this->gCopyFrameTexture[0] = createTexture(width, height);
    this->gCopyFrameTexture[1] = createTexture(width, height);
    attachTextureNum(this->gCopyFrameFBO, this->gCopyFrameTexture[0], 0);
    attachTextureNum(this->gCopyFrameFBO, this->gCopyFrameTexture[1], 0);

    this->coverage = 0.45;

    this->perlinTex = createPerlinTexture();
    this->worley32 = createWorleyTexture();
    this->weatherTex = createWeatherTexture();
}

GLuint CloudSystem::createPerlinTexture()
{
    std::vector<glm::vec3> perlinNoise;
    for (int i = 0; i < 128 * 128 * 128; i++) {
        glm::vec3 noise(generateFloat() * 2.0 - 1.0, generateFloat() * 2.0 - 1.0, 0.0f);
        perlinNoise.push_back(noise);
    }
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_3D, texture_id);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, 128, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, &perlinNoise[0]);
    glBindTexture(GL_TEXTURE_3D, 0);
    return texture_id;
}

GLuint CloudSystem::createWorleyTexture()
{
    std::vector<glm::vec3> worleyNoise;
    for (int i = 0; i < 32 * 32 * 32; i++) {
        glm::vec3 noise(generateFloat() * 2.0 - 1.0, generateFloat() * 2.0 - 1.0, 0.0f);
        worleyNoise.push_back(noise);
    }
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_3D, texture_id);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, 32, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, &worleyNoise[0]);
    glBindTexture(GL_TEXTURE_3D, 0);
    return texture_id;
}

GLuint CloudSystem::createWeatherTexture()
{
    std::vector<glm::vec3> weatherNoise;
    for (int i = 0; i < 1024 * 1024; i++) {
        glm::vec3 noise(generateFloat() * 2.0 - 1.0, generateFloat() * 2.0 - 1.0, 0.0f);
        weatherNoise.push_back(noise);
    }
    GLuint noiseTexture;
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_FLOAT, &weatherNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
    return noiseTexture;
}

CloudSystem::~CloudSystem()
{
}

void CloudSystem::render(EngineSettings* engineSettings)
{
    engineSettings->gPostProcess->stop();
    glBindFramebuffer(GL_FRAMEBUFFER, this->gCloudFBO);

    const GLenum buffers[]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, buffers);

    float bkColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, bkColor);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glClearColor(bkColor[0], bkColor[1], bkColor[2], bkColor[3]);

    this->volumetricCloudShader->use();
    this->volumetricCloudShader->setVec2("iResolution", glm::vec2(engineSettings->window->width, engineSettings->window->height));
    this->volumetricCloudShader->setFloat("iTime", glfwGetTime());
    this->volumetricCloudShader->setMat4("inv_proj", glm::inverse(engineSettings->projectionMatrix));
    this->volumetricCloudShader->setMat4("inv_view", glm::inverse(engineSettings->viewMatrix));
    this->volumetricCloudShader->setVec3("cameraPosition", engineSettings->camera->Position);
    this->volumetricCloudShader->setFloat("FOV", engineSettings->camera->Zoom);
    //this->volumetricCloudShader->setVec3("lightDirection", glm::normalize(s->lightPos - s->cam.Position));
    //this->volumetricCloudShader->setVec3("lightColor", s->lightColor);
    this->volumetricCloudShader->setVec3("lightDirection", glm::vec3(1, 1, 1));
    this->volumetricCloudShader->setVec3("lightColor", glm::vec3(1, 0, 0));
    this->volumetricCloudShader->setFloat("coverage_multiplier", coverage);
    this->volumetricCloudShader->setInt("frameIter", this->frameIter);
    glm::mat4 vp = engineSettings->projectionMatrix * engineSettings->viewMatrix;

    this->volumetricCloudShader->setMat4("invViewProj", glm::inverse(vp));
    this->volumetricCloudShader->setMat4("oldFrameVP", oldFrameVP);
    this->volumetricCloudShader->setMat4("gVP", vp);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, this->perlinTex);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, this->worley32);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->weatherTex);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, engineSettings->gGBuffer->gDepthTexture);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, this->gCopyFrameTexture[0]);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, this->gCopyFrameTexture[1]);

    this->volumetricCloudShader->setInt("cloud", 0);
    this->volumetricCloudShader->setInt("worley32", 1);
    this->volumetricCloudShader->setInt("weatherTex", 2);
    this->volumetricCloudShader->setInt("depthMap", 3);
    this->volumetricCloudShader->setInt("lastFrameAlphaness", 4);
    this->volumetricCloudShader->setInt("lastFrameColor", 5);

    //actual draw
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_3D, 0);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_3D, 0);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    //copy to lastFrameFBO
    glBindFramebuffer(GL_FRAMEBUFFER, this->gCopyFrameFBO);

    const GLenum buffers2[]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, buffers2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->gCloudTextures[2]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->gCloudTextures[1]);

    this->copyFrameShader->use();
    this->volumetricCloudShader->setInt("colorTex", 0);
    this->volumetricCloudShader->setInt("alphanessTex", 1);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    //copy last VP matrix
    this->oldFrameVP = vp;

    //increment frame counter mod 16, for temporal reprojection
    frameIter = (frameIter + 1) % 16;

    engineSettings->gPostProcess->start();
}
