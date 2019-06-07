#include "graphics/effects/lightscatteringeffect.hpp"

//https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch13.html
//https://www.fabiensanglard.net/lightScattering/index.php
//https://github.com/Erkaman/glsl-godrays

LightScatteringEffect::LightScatteringEffect()
{
    this->lightScatteringShader = new Shader("assets/shaders/lightscattering.vs", "assets/shaders/lightscattering.fs");

    glGenVertexArrays(1, &this->VAO);

    this->lightScatteringShader->use();
    this->lightScatteringShader->setFloat("exposure", 0.5f); //0.0034f overall instensity
    this->lightScatteringShader->setFloat("decay", 0.99f); //1.0f range 0-1 falloff of samples
    this->lightScatteringShader->setFloat("density", 1.0f); //0.84f distance between samples
    this->lightScatteringShader->setFloat("weight", 0.01f); //5.65f intensity of each sample
    this->lightScatteringShader->setInt("samples", 50); //5.65f intensity of each sample
    //this->lightScatteringShader->setVec2("lightPositionOnScreen", 0.5f, 0.5f);
}

LightScatteringEffect::~LightScatteringEffect()
{
    delete this->lightScatteringShader;
}

inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from)
{
    glm::mat4 to;
    // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = from.a1;
    to[1][0] = from.a2;
    to[2][0] = from.a3;
    to[3][0] = from.a4;
    to[0][1] = from.b1;
    to[1][1] = from.b2;
    to[2][1] = from.b3;
    to[3][1] = from.b4;
    to[0][2] = from.c1;
    to[1][2] = from.c2;
    to[2][2] = from.c3;
    to[3][2] = from.c4;
    to[0][3] = from.d1;
    to[1][3] = from.d2;
    to[2][3] = from.d3;
    to[3][3] = from.d4;
    return to;
}

void LightScatteringEffect::render(Window* window, ModelEntity* lightSource, GBuffer* gBuffer, PostProcess* postProcess)
{
    postProcess->start();

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postProcess->getTexture());

    glDisable(GL_DEPTH_TEST);

    this->lightScatteringShader->use();

    glm::mat4 modelMatrix = aiMatrix4x4ToGlm(lightSource->ourModel->rootMatrix);
    modelMatrix = glm::translate(modelMatrix, *lightSource->position);
    modelMatrix = glm::scale(modelMatrix, *lightSource->scale);

    glm::mat4 modelViewMatrix = gBuffer->viewMatrix * modelMatrix;

    //gluProject(lightSource->position->x, lightSource->position->y, lightSource->position->z, modelMatrix, gBuffer->projectionMatrix.data(), &gBuffer->viewMatrix, &windowXPos, &windowYPos, &windowZPos);
    //gluProject(lightSource->position->x, lightSource->position->y, lightSource->position->z, modelMatrix, gBuffer->projectionMatrix.data(), &gBuffer->viewMatrix, &windowXPos, &windowYPos, &windowZPos);
    glm::vec4 viewport = glm::vec4(0.0f, 0.0f, (float)window->width, (float)window->height);
    glm::vec3 windowCoords = glm::project(*lightSource->position, modelViewMatrix, gBuffer->projectionMatrix, viewport);
    this->lightScatteringShader->setVec2("lightPositionOnScreen", windowCoords.x / window->width, windowCoords.y / window->height);

    float exposureClean = 0.5f;
    if (windowCoords.z > 1.0f)
        exposureClean = 0.0f;

    if (abs(windowCoords.x) > window->width)
        exposureClean -= (abs(windowCoords.x) / window->width) - 1.0f;

    if (abs(windowCoords.y) > window->height)
        exposureClean -= (abs(windowCoords.y) / window->height) - 1.0f;

    this->lightScatteringShader->setFloat("exposure", exposureClean);
    //this->lightScatteringShader->setVec2("lightPositionOnScreen", windowCoords.x, windowCoords.y);
    this->lightScatteringShader->setInt("uColorTexture", 0);

    //std::cout << "XPos: " << windowCoords.x << " , YPos: " << windowCoords.y << " , ZPos: " << windowCoords.z << std::endl;

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    postProcess->swapBuffer();
}
