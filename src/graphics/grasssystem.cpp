#include "graphics/grasssystem.hpp"

//https://learnopengl.com/Advanced-Lighting/Bloom

GrassSystem::GrassSystem()
{
    glGenVertexArrays(1, &this->VAO);

    for (int loopNum = 0; loopNum <= 2; loopNum++) {
        std::string geomLocation = "assets/shaders/grass/grass_lod" + std::to_string(loopNum + 1) + ".gs";
        this->grassLODShader[loopNum] = new Shader("assets/shaders/grass/grass.vs", "assets/shaders/grass/grass.fs", geomLocation.c_str());
    }
}

GrassSystem::~GrassSystem()
{
    for (int loopNum = 0; loopNum <= 2; loopNum++) {
        delete this->grassLODShader[loopNum];
    }
}

float windTime = 0.0f;
void GrassSystem::render(EngineSettings* engineSettings, TransvoxelSystem* transvoxel)
{
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    Shader* grassShader = this->grassLODShader[0];

    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, postProcess->getTexture());
    grassShader->use();

    //windTime += 0.00001;

    grassShader->setVec3("color", 1.0f, 0.0f, 0.0f);
    grassShader->setFloat("shininess", 80.0f);
    grassShader->setFloat("wind_strength", 1.0f);
    grassShader->setVec3("wind_direction", 0.3f, 0.3f, 0.3f);
    grassShader->setVec2("time_offset", 1.0f, windTime);
    //glUniform3fv(glGetUniformLocation(grass_shader, "color"), 1, value_ptr(lod.color));
    //glUniform1f(glGetUniformLocation(grass_shader, "shininess"), 80.f);
    //glUniform1f(glGetUniformLocation(grass_shader, "wind_strength"), 1.f);
    //vec3 wind_dir = vec3(camera.get_view_matrix() * vec4(0.3f, 0.f, -0.7f, 0.f));
    //glUniform3fv(glGetUniformLocation(grass_shader, "wind_direction"), 1, value_ptr(wind_dir));
    //glUniform2fv(glGetUniformLocation(grass_shader, "time_offset"), 1, value_ptr(((float)SDL_GetTicks()) / 40000.f * vec2(0.8f, -0.1f)));

    // FOR EACH TERRAIN

    grassShader->setMat4("projection", engineSettings->projectionMatrix);
    grassShader->setMat4("view", engineSettings->viewMatrix);
    grassShader->setMat4("model", transvoxel->modelMatrix);

    glBindVertexArray(transvoxel->volumeVAO);
    glDrawArrays(GL_TRIANGLES, 0, transvoxel->trianglesVector.size());
    //glDrawElements(GL_TRIANGLES, 1, GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);

    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}