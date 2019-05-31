#pragma once
#ifndef MODEL_ENTITY_HPP_
#define MODEL_ENTITY_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/enginesettings.hpp"
#include "core/entity.hpp"
#include "graphics/model.hpp"
#include "graphics/shader.hpp"

class ModelEntity : public Entity {
public:
    ModelEntity(const char* vertex, const char* fragment, const char* model);
    ~ModelEntity();
    void render(EngineSettings* engineSettings);
    void update(EngineSettings* engineSettings);
    void setPosition(float x, float y, float z);
    void setScale(float x, float y, float z);
    Shader* ourShader = nullptr;
    Model* ourModel = nullptr;
    glm::vec3* position = new glm::vec3(0.0, 0.0, 0.0);
    glm::vec3* scale = new glm::vec3(1.0, 1.0, 1.0);
};

#endif // MODEL_ENTITY_HPP_