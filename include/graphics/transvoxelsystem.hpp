#pragma once
#ifndef TRANSVOXEL_SYSTEM_HPP_
#define TRANSVOXEL_SYSTEM_HPP_

#include <noise/module/modulebase.h>
#include <noise/noise.h>

using namespace noise;

// Local Headers
#include "core/enginesettings.hpp"
#include "graphics/marchingcubes.h"
#include "graphics/shader.hpp"
#include "graphics/transvoxel.h"

// System Headers
#define GLEW_STATIC
#include <GL/glew.h> // This must appear before freeglut.h
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <ctime>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Voxel {
    float isovalue = 0;
};

struct VoxelCube {
    glm::vec3 pos;
    float isovalue;
};

class TransvoxelSystem {
private:
    // Scene* tempScene;
    Shader* volumeShader = nullptr;
    float t = 0;
    int shapeIndex = 1;
    int voxelScale = 2;
    int editingIsovalue = 10;
    float cameraSpeedFactor = 1.0f;
    bool isRotating = false;
    bool isWireFrame = false;
    bool isCursorDisabled = true;
    bool lodSmoothing = true;
    bool usingTransvoxels = true;
    void updateIsovalues(int x, int y, int z, float isovalue, int radius);
    VoxelCube voxelForVolumePos(int x, int y, int z);
    glm::vec3 gradientForPoint(int x, int y, int z);
    glm::vec3 interpolateVertex(float isorange, VoxelCube v1, VoxelCube v2);
    glm::vec3 interpolateNormal(int x, int y, int z, int isorange, int corner1, int corner2);
    glm::vec3 interpolateTransNormal(int x, int y, int z, int isorange, int corner1, int corner2);
    void setupVolumeIsovalues();
    void computeTrianglesForVoxel(int x, int y, int z);
    void computeTrianglesForTransvoxel(int x, int y, int z);
    void setupVolumeData();
    void resetVolumeData();
    void setupVolumeTriangles();
    float getIsovalueFor3DSimplexNoise(int x, int y, int z);
    module::Perlin myModule;

public:
    TransvoxelSystem(int size);
    ~TransvoxelSystem();
    void drawVolumeData(EngineSettings* engineSettings, float aspect);
    const int size;
    std::vector<Voxel> volumeData;
    GLuint VBO, VAO;
    GLuint volumeVBO, volumeVAO;
    std::vector<float> trianglesVector;
    std::vector<float> normalsVector;
    glm::mat4 modelMatrix;
};

#endif // TRANSVOXEL_SYSTEM_HPP_