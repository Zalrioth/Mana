#include "graphics/transvoxelsystem.hpp"

//https://stackoverflow.com/questions/30501919/error-lnk2005-already-defined-in-obj
//https://stackoverflow.com/questions/2785612/c-what-does-the-colon-after-a-constructor-mean
//http://www.c-jump.com/bcc/common/Talk3/Math/GLM/GLM.html

TransvoxelSystem::TransvoxelSystem(int size)
    : size(size)
{
    Voxel example;
    volumeData.resize(size * size * size, example);
    setupVolumeData();

    this->volumeShader = new Shader("assets/shaders/volume.vert", "assets/shaders/volume.frag");
}

TransvoxelSystem::~TransvoxelSystem()
{
    // Maybe delete some stuff
}

void TransvoxelSystem::updateIsovalues(int x, int y, int z, float isovalue,
    int radius)
{
    for (int xd = -radius; xd <= radius; xd++) {
        for (int yd = -radius; yd <= radius; yd++) {
            for (int zd = -radius; zd <= radius; zd++) {
                if (xd * xd + yd * yd + zd * zd > radius * radius)
                    continue;
                int index = ((x + xd) * size * size + (y + yd) * size + (z + zd));
                if (index >= 0 && index < size * size * size) {
                    volumeData[index].isovalue = isovalue;
                }
            }
        }
    }
    //    for (int xd = -(radius+1); xd <= radius+1; xd++) {
    //        for (int yd = -(radius+1); yd <= radius+1; yd++) {
    //            for (int zd = -(radius+1); zd <= radius+1; zd++) {
    //                recomputeVertices(x + xd, y + yd, z + zd);
    //            }
    //        }
    //    }
}

VoxelCube TransvoxelSystem::voxelForVolumePos(int x, int y, int z)
{
    glm::vec3 pos(x, y, z);
    Voxel voxel = volumeData[x * size * size + y * size + z];
    return { pos, voxel.isovalue };
}

glm::vec3 TransvoxelSystem::gradientForPoint(int x, int y, int z)
{
    int minx = (x - voxelScale >= 0) ? x - voxelScale : x;
    int maxx = (x + voxelScale <= size - 1) ? x + voxelScale : x;
    float divisorx = (x == 0 || x == size - 1) ? 1.0f : 2.0f;
    float gradx = (voxelForVolumePos(maxx, y, z).isovalue - voxelForVolumePos(minx, y, z).isovalue) / divisorx;
    // grad[0] = (maxx - minx) / divisorx;

    int miny = (y - voxelScale >= 0) ? y - voxelScale : y;
    int maxy = (y + voxelScale <= size - 1) ? y + voxelScale : y;
    float divisory = (y == 0 || y == size - 1) ? 1.0f : 2.0f;
    float grady = (voxelForVolumePos(x, maxy, z).isovalue - voxelForVolumePos(x, miny, z).isovalue) / divisory;
    // grad[1] = (maxy - miny) / divisory;

    int minz = (z - voxelScale >= 0) ? z - voxelScale : z;
    int maxz = (z + voxelScale <= size - 1) ? z + voxelScale : z;
    float divisorz = (z == 0 || z == size - 1) ? 1.0f : 2.0f;
    float gradz = (voxelForVolumePos(x, y, maxz).isovalue - voxelForVolumePos(x, y, minz).isovalue) / divisorz;
    // grad[2] = (maxz - minz) / divisorz;

    return glm::vec3(gradx, grady, gradz);
}

glm::vec3 TransvoxelSystem::interpolateVertex(float isorange, VoxelCube v1,
    VoxelCube v2)
{
    glm::vec3 p1 = v1.pos;
    glm::vec3 p2 = v2.pos;
    float iso1 = v1.isovalue;
    float iso2 = v2.isovalue;
    if (p2[0] < p1[0] || p2[1] < p1[1] || p2[2] < p1[2]) {
        glm::vec3 temp = p1;
        p1 = p2;
        p2 = temp;
        float temp2 = iso1;
        iso1 = iso2;
        iso2 = temp2;
    }

    if (abs(iso1 - iso2) > 0.00001) {
        return p1 + (p2 - p1) / (iso2 - iso1) * (isorange - iso1);
    } else {
        return p1;
    }
}

glm::vec3 TransvoxelSystem::interpolateNormal(int x, int y, int z, int isorange,
    int corner1, int corner2)
{
    // Get gradient for each corner
    glm::vec3 grad[2];
    VoxelCube voxels[2];
    for (int i = 0; i < 2; i++) {
        int corner = (i == 0) ? corner1 : corner2;
        int cornerOffset[3] = { 0 };
        cornerOffset[0] = cornerOffsets[corner][0] * voxelScale;
        cornerOffset[1] = cornerOffsets[corner][1] * voxelScale;
        cornerOffset[2] = cornerOffsets[corner][2] * voxelScale;
        grad[i] = gradientForPoint(x + cornerOffsets[corner][0] * voxelScale,
            y + cornerOffsets[corner][1] * voxelScale,
            z + cornerOffsets[corner][2] * voxelScale);
        voxels[i] = voxelForVolumePos(x + cornerOffsets[corner][0] * voxelScale,
            y + cornerOffsets[corner][1] * voxelScale,
            z + cornerOffsets[corner][2] * voxelScale);
    }

    glm::vec3 grad1 = grad[0];
    glm::vec3 grad2 = grad[1];
    float iso1 = voxels[0].isovalue;
    float iso2 = voxels[1].isovalue;
    if (iso2 < iso1) {
        float temp = iso1;
        iso1 = iso2;
        iso2 = temp;
        glm::vec3 temp2 = grad1;
        grad1 = grad2;
        grad2 = temp2;
    }

    if (abs(iso1 - iso2) > 0.00001) {
        return grad1 + (grad2 - grad1) / (iso2 - iso1) * (isorange - iso1);
    } else {
        return grad1;
    }
}

glm::vec3 TransvoxelSystem::interpolateTransNormal(int x, int y, int z,
    int isorange, int corner1,
    int corner2)
{
    // Get gradient for each corner
    glm::vec3 grad[2];
    VoxelCube voxels[2];
    for (int i = 0; i < 2; i++) {
        int corner = (i == 0) ? corner1 : corner2;
        int cornerOffset[3] = { 0 };
        cornerOffset[0] = transCornerOffsets[corner][0] * voxelScale / 2; // Divide by 2 because offset table uses values 2x as large
        cornerOffset[1] = transCornerOffsets[corner][1] * voxelScale / 2;
        cornerOffset[2] = transCornerOffsets[corner][2] * voxelScale / 2;
        grad[i] = gradientForPoint(x + cornerOffset[0], y + cornerOffset[1],
            z + cornerOffset[2]);
        voxels[i] = voxelForVolumePos(x + cornerOffset[0], y + cornerOffset[1],
            z + cornerOffset[2]);
    }

    glm::vec3 grad1 = grad[0];
    glm::vec3 grad2 = grad[1];
    float iso1 = voxels[0].isovalue;
    float iso2 = voxels[1].isovalue;
    if (iso2 < iso1) {
        float temp = iso1;
        iso1 = iso2;
        iso2 = temp;
        glm::vec3 temp2 = grad1;
        grad1 = grad2;
        grad2 = temp2;
    }

    if (abs(iso1 - iso2) > 0.00001) {
        return grad1 + (grad2 - grad1) / (iso2 - iso1) * (isorange - iso1);
    } else {
        return grad1;
    }
}

void TransvoxelSystem::setupVolumeIsovalues()
{
    // Frequency: increasing shrinks noise map
    // Scale: increasing shrinks sample step
    //module::Perlin myModule;
    //module::RidgedMulti myModule;
    //module::Voronoi myModule;

    //TODO: Implement user seed this is just to get random noise
    this->myModule.SetSeed(time(NULL));
    this->myModule.SetOctaveCount(4);
    this->myModule.SetFrequency(4.0);
#pragma omp parallel for collapse(3)
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            for (int z = 0; z < size; z++) {
                int index = (x * size * size + y * size + z);
                volumeData[index].isovalue = getIsovalueFor3DSimplexNoise(x, y, z);
            }
        }
    }
}

void TransvoxelSystem::computeTrianglesForVoxel(int x, int y, int z)
{
    glm::vec3 cornerPositions[8] = {
        glm::vec3(x - voxelScale, y - voxelScale, z - voxelScale),
        glm::vec3(x, y - voxelScale, z - voxelScale),
        glm::vec3(x - voxelScale, y, z - voxelScale),
        glm::vec3(x, y, z - voxelScale),
        glm::vec3(x - voxelScale, y - voxelScale, z),
        glm::vec3(x, y - voxelScale, z),
        glm::vec3(x - voxelScale, y, z),
        glm::vec3(x, y, z)
    };

    int cornerIndices[8] = {
        (x - voxelScale) * size * size + (y - voxelScale) * size + (z - voxelScale),
        (x)*size * size + (y - voxelScale) * size + (z - voxelScale),
        (x - voxelScale) * size * size + (y)*size + (z - voxelScale),
        (x)*size * size + (y)*size + (z - voxelScale),
        (x - voxelScale) * size * size + (y - voxelScale) * size + (z),
        (x)*size * size + (y - voxelScale) * size + (z),
        (x - voxelScale) * size * size + (y)*size + (z),
        (x)*size * size + (y)*size + (z)
    };

    unsigned long cubeIndex = 0;
    float isorange = 0;
    if ((volumeData[cornerIndices[0]].isovalue) <= isorange)
        cubeIndex |= 1;
    if ((volumeData[cornerIndices[1]].isovalue) <= isorange)
        cubeIndex |= 2;
    if ((volumeData[cornerIndices[2]].isovalue) <= isorange)
        cubeIndex |= 4;
    if ((volumeData[cornerIndices[3]].isovalue) <= isorange)
        cubeIndex |= 8;
    if ((volumeData[cornerIndices[4]].isovalue) <= isorange)
        cubeIndex |= 16;
    if ((volumeData[cornerIndices[5]].isovalue) <= isorange)
        cubeIndex |= 32;
    if ((volumeData[cornerIndices[6]].isovalue) <= isorange)
        cubeIndex |= 64;
    if ((volumeData[cornerIndices[7]].isovalue) <= isorange)
        cubeIndex |= 128;

    /* Cube is entirely in/out of the surface */
    if (edgeTable[cubeIndex] == 0)
        return;

    /* Transvoxel */
    auto cellClass = regularCellClass[cubeIndex];
    auto cellData = regularCellData[cellClass];
    unsigned short vertexData[12] = { 0 };
    for (int i = 0; i < 12; i++) {
        vertexData[i] = regularVertexData[cubeIndex][i];
    }

    /* Find the vertices where the surface intersects the cube for each edge */
    std::vector<glm::vec3> edgeVertices;
    std::vector<glm::vec3> edgeNormals;
    for (int i = 0; i < cellData.GetVertexCount(); i++) {
        auto vertex = vertexData[i];
        if (!vertex)
            break;
        unsigned char corner1 = (vertex >> 4) & 0x000F;
        unsigned char corner2 = vertex & 0x000F;
        float isovalue1 = volumeData[cornerIndices[corner1]].isovalue;
        float isovalue2 = volumeData[cornerIndices[corner2]].isovalue;
        glm::vec3 cornerPos1 = cornerPositions[corner1];
        glm::vec3 cornerPos2 = cornerPositions[corner2];
        if (lodSmoothing) {
            int lod = voxelScale;
            while (lod > 1) {
                glm::vec3 centerVoxelPosition = (cornerPos1 + cornerPos2) / 2.0f;
                int centerVoxelIndex = (int)centerVoxelPosition[0] * size * size + (int)centerVoxelPosition[1] * size + (int)centerVoxelPosition[2];
                Voxel centerVoxel = volumeData[centerVoxelIndex];
                if (centerVoxel.isovalue <= 0) {
                    if (isovalue1 <= 0) {
                        isovalue1 = centerVoxel.isovalue;
                        cornerPos1 = centerVoxelPosition;
                    } else {
                        isovalue2 = centerVoxel.isovalue;
                        cornerPos2 = centerVoxelPosition;
                    }
                } else {
                    if (isovalue1 > 0) {
                        isovalue1 = centerVoxel.isovalue;
                        cornerPos1 = centerVoxelPosition;
                    } else {
                        isovalue2 = centerVoxel.isovalue;
                        cornerPos2 = centerVoxelPosition;
                    }
                }
                lod /= 2;
            }
        }
        float t = isovalue2 / (isovalue2 - isovalue1);
        glm::vec3 vertexPos = cornerPos1 * t + cornerPos2 * (1 - t);
        edgeVertices.push_back(vertexPos);
        edgeNormals.push_back(
            interpolateNormal(x, y, z, isorange, corner1, corner2));
    }

    /* Create the triangles */
    for (int i = 0; i < cellData.GetTriangleCount() * 3; i++) {
        auto vertexIndex = cellData.vertexIndex[i];
        for (int j = 0; j < 3; j++) {
            trianglesVector.push_back(edgeVertices[vertexIndex][j]);
            normalsVector.push_back(edgeNormals[vertexIndex][j]);
        }
    }
}

void TransvoxelSystem::computeTrianglesForTransvoxel(int x, int y, int z)
{
    // Which axes contains the higher LOD. Bit corresponds to (00-z-y-xzyx)
    int axis = 1 << 5;

    glm::vec3 cornerPositions[8] = {
        glm::vec3(x - voxelScale, y - voxelScale, z - voxelScale),
        glm::vec3(x, y - voxelScale, z - voxelScale),
        glm::vec3(x - voxelScale, y, z - voxelScale),
        glm::vec3(x, y, z - voxelScale),
        glm::vec3(x - voxelScale, y - voxelScale, z),
        glm::vec3(x, y - voxelScale, z),
        glm::vec3(x - voxelScale, y, z),
        glm::vec3(x, y, z)
    };

    int cornerIndices[8] = {
        (x - voxelScale) * size * size + (y - voxelScale) * size + (z - voxelScale),
        (x)*size * size + (y - voxelScale) * size + (z - voxelScale),
        (x - voxelScale) * size * size + (y)*size + (z - voxelScale),
        (x)*size * size + (y)*size + (z - voxelScale),
        (x - voxelScale) * size * size + (y - voxelScale) * size + (z),
        (x)*size * size + (y - voxelScale) * size + (z),
        (x - voxelScale) * size * size + (y)*size + (z),
        (x)*size * size + (y)*size + (z)
    };

    unsigned long cubeIndex = 0;
    float isorange = 0;
    if ((volumeData[cornerIndices[0]].isovalue) <= isorange)
        cubeIndex |= 1;
    if ((volumeData[cornerIndices[1]].isovalue) <= isorange)
        cubeIndex |= 2;
    if ((volumeData[cornerIndices[2]].isovalue) <= isorange)
        cubeIndex |= 4;
    if ((volumeData[cornerIndices[3]].isovalue) <= isorange)
        cubeIndex |= 8;
    if ((volumeData[cornerIndices[4]].isovalue) <= isorange)
        cubeIndex |= 16;
    if ((volumeData[cornerIndices[5]].isovalue) <= isorange)
        cubeIndex |= 32;
    if ((volumeData[cornerIndices[6]].isovalue) <= isorange)
        cubeIndex |= 64;
    if ((volumeData[cornerIndices[7]].isovalue) <= isorange)
        cubeIndex |= 128;

    /* Cube is entirely in/out of the surface */
    if (edgeTable[cubeIndex] == 0)
        return;

    /* Transvoxel */
    auto cellClass = regularCellClass[cubeIndex];
    auto cellData = regularCellData[cellClass];
    unsigned short vertexData[12] = { 0 };
    for (int i = 0; i < 12; i++) {
        vertexData[i] = regularVertexData[cubeIndex][i];
    }

    /* Find the vertices where the surface intersects the cube for each edge */
    std::vector<glm::vec3> edgeVertices;
    std::vector<glm::vec3> edgeNormals;
    for (int i = 0; i < cellData.GetVertexCount(); i++) {
        auto vertex = vertexData[i];
        if (!vertex)
            break;
        unsigned char corner1 = (vertex >> 4) & 0x000F;
        unsigned char corner2 = vertex & 0x000F;
        float isovalue1 = volumeData[cornerIndices[corner1]].isovalue;
        float isovalue2 = volumeData[cornerIndices[corner2]].isovalue;
        glm::vec3 cornerPos1 = cornerPositions[corner1];
        glm::vec3 cornerPos2 = cornerPositions[corner2];
        if (lodSmoothing) {
            int lod = voxelScale;
            while (lod > 1) {
                glm::vec3 centerVoxelPosition = (cornerPos1 + cornerPos2) / 2.0f;
                int centerVoxelIndex = (int)centerVoxelPosition[0] * size * size + (int)centerVoxelPosition[1] * size + (int)centerVoxelPosition[2];
                Voxel centerVoxel = volumeData[centerVoxelIndex];
                if (centerVoxel.isovalue <= 0) {
                    if (isovalue1 <= 0) {
                        isovalue1 = centerVoxel.isovalue;
                        cornerPos1 = centerVoxelPosition;
                    } else {
                        isovalue2 = centerVoxel.isovalue;
                        cornerPos2 = centerVoxelPosition;
                    }
                } else {
                    if (isovalue1 > 0) {
                        isovalue1 = centerVoxel.isovalue;
                        cornerPos1 = centerVoxelPosition;
                    } else {
                        isovalue2 = centerVoxel.isovalue;
                        cornerPos2 = centerVoxelPosition;
                    }
                }
                lod /= 2;
            }
        }
        // Scale the normal cell to be half the volume size along the transvoxel
        // axes
        // TODO: Implement the other cases
        if (axis == 1 << 5) { // Negative z axis
            if (corner1 < 4) {
                cornerPos1.z += voxelScale * 0.5f;
            }
            if (corner2 < 4) {
                cornerPos2.z += voxelScale * 0.5f;
            }
        }
        float t = isovalue2 / (isovalue2 - isovalue1);
        glm::vec3 vertexPos = cornerPos1 * t + cornerPos2 * (1 - t);
        edgeVertices.push_back(vertexPos);
        edgeNormals.push_back(
            interpolateNormal(x, y, z, isorange, corner1, corner2));
    }

    /* Create the triangles */
    for (int i = 0; i < cellData.GetTriangleCount() * 3; i++) {
        auto vertexIndex = cellData.vertexIndex[i];
        for (int j = 0; j < 3; j++) {
            trianglesVector.push_back(edgeVertices[vertexIndex][j]);
            normalsVector.push_back(edgeNormals[vertexIndex][j]);
        }
    }

    // TRANSVOXEL PART (-z axis specific)
    glm::vec3 transCornerPositions[13] = {
        glm::vec3(x - voxelScale, y - voxelScale, z - voxelScale),
        glm::vec3(x - voxelScale / 2, y - voxelScale, z - voxelScale),
        glm::vec3(x, y - voxelScale, z - voxelScale),
        glm::vec3(x - voxelScale, y - voxelScale / 2, z - voxelScale),
        glm::vec3(x - voxelScale / 2, y - voxelScale / 2, z - voxelScale),
        glm::vec3(x, y - voxelScale / 2, z - voxelScale),
        glm::vec3(x - voxelScale, y, z - voxelScale),
        glm::vec3(x - voxelScale / 2, y, z - voxelScale),
        glm::vec3(x, y, z - voxelScale),
        glm::vec3(x - voxelScale, y - voxelScale, z - voxelScale),
        glm::vec3(x, y - voxelScale, z - voxelScale),
        glm::vec3(x - voxelScale, y, z - voxelScale),
        glm::vec3(x, y, z - voxelScale),
    };

    /*std::cout << "Trans Corner Positions:" << std::endl;
  for (int i = 0; i < 13; i++) {
          std::cout << i << ": (" << transCornerPositions[i][0] << ", " <<
  transCornerPositions[i][1] << ", " << transCornerPositions[i][2] << ")" <<
  std::endl;
  }*/

    int transCornerIndices[13] = {
        (x - voxelScale) * size * size + (y - voxelScale) * size + (z - voxelScale),
        (x - voxelScale / 2) * size * size + (y - voxelScale) * size + (z - voxelScale),
        (x)*size * size + (y - voxelScale) * size + (z - voxelScale),
        (x - voxelScale) * size * size + (y - voxelScale / 2) * size + (z - voxelScale),
        (x - voxelScale / 2) * size * size + (y - voxelScale / 2) * size + (z - voxelScale),
        (x)*size * size + (y - voxelScale / 2) * size + (z - voxelScale),
        (x - voxelScale) * size * size + (y)*size + (z - voxelScale),
        (x - voxelScale / 2) * size * size + (y)*size + (z - voxelScale),
        (x)*size * size + (y)*size + (z - voxelScale),
        (x - voxelScale) * size * size + (y - voxelScale) * size + (z - voxelScale),
        (x)*size * size + (y - voxelScale) * size + (z - voxelScale),
        (x - voxelScale) * size * size + (y)*size + (z - voxelScale),
        (x)*size * size + (y)*size + (z - voxelScale)
    };

    unsigned long transCubeIndex = 0;
    if ((volumeData[transCornerIndices[0]].isovalue) <= isorange)
        transCubeIndex |= 1;
    if ((volumeData[transCornerIndices[1]].isovalue) <= isorange)
        transCubeIndex |= 2;
    if ((volumeData[transCornerIndices[2]].isovalue) <= isorange)
        transCubeIndex |= 4;
    if ((volumeData[transCornerIndices[3]].isovalue) <= isorange)
        transCubeIndex |= 128;
    if ((volumeData[transCornerIndices[4]].isovalue) <= isorange)
        transCubeIndex |= 256;
    if ((volumeData[transCornerIndices[5]].isovalue) <= isorange)
        transCubeIndex |= 8;
    if ((volumeData[transCornerIndices[6]].isovalue) <= isorange)
        transCubeIndex |= 64;
    if ((volumeData[transCornerIndices[7]].isovalue) <= isorange)
        transCubeIndex |= 32;
    if ((volumeData[transCornerIndices[8]].isovalue) <= isorange)
        transCubeIndex |= 16;

    /* Cube is entirely in/out of the surface */
    if (transCubeIndex == 0 || transCubeIndex == 511)
        return;

    /* Transvoxel */
    auto transCellClass = transitionCellClass[transCubeIndex];
    auto transCellData = transitionCellData[transCellClass & 0x7F];
    unsigned short transVertexData[12] = { 0 };
    for (int i = 0; i < 12; i++) {
        transVertexData[i] = transitionVertexData[transCubeIndex][i];
    }

    /* Find the vertices where the surface intersects the cube for each edge */
    std::vector<glm::vec3> transEdgeVertices;
    std::vector<glm::vec3> transEdgeNormals;
    // std::cout << "Vertex/Triangle Count: " << transCellData.GetVertexCount() <<
    // " / " << transCellData.GetTriangleCount() << std::endl;
    for (int i = 0; i < transCellData.GetVertexCount(); i++) {
        unsigned short vertex = transVertexData[i];
        if (!vertex)
            break;
        unsigned char corner1 = (vertex >> 4) & 0x000F;
        unsigned char corner2 = vertex & 0x000F;
        // std::cout << "Corners: (" << (int)corner1 << "," << (int)corner2 << ")";
        float isovalue1 = volumeData[transCornerIndices[corner1]].isovalue;
        float isovalue2 = volumeData[transCornerIndices[corner2]].isovalue;
        // std::cout << " Iso: (" << isovalue1 << "," << isovalue2 << ")";
        glm::vec3 cornerPos1 = transCornerPositions[corner1];
        glm::vec3 cornerPos2 = transCornerPositions[corner2];
        if (lodSmoothing) {
            int lod = voxelScale;
            while (lod > 1) {
                glm::vec3 centerVoxelPosition = (cornerPos1 + cornerPos2) / 2.0f;
                int centerVoxelIndex = (int)centerVoxelPosition[0] * size * size + (int)centerVoxelPosition[1] * size + (int)centerVoxelPosition[2];
                Voxel centerVoxel = volumeData[centerVoxelIndex];
                if (centerVoxel.isovalue <= 0) {
                    if (isovalue1 <= 0) {
                        if (corner1 < 9) { // Corner is on high-res face && LOD > 2
                            if (lod > 2) {
                                isovalue1 = centerVoxel.isovalue;
                                cornerPos1 = centerVoxelPosition;
                            }
                        } else { // Else it's ok for LOD to be 2
                            isovalue1 = centerVoxel.isovalue;
                            cornerPos1 = centerVoxelPosition;
                        }
                    } else {
                        if (corner2 < 9) { // Corner is on high-res face && LOD > 2
                            if (lod > 2) {
                                isovalue2 = centerVoxel.isovalue;
                                cornerPos2 = centerVoxelPosition;
                            }
                        } else { // Else it's ok for LOD to be 2
                            isovalue2 = centerVoxel.isovalue;
                            cornerPos2 = centerVoxelPosition;
                        }
                    }
                } else {
                    if (isovalue1 > 0) {
                        if (corner1 < 9) { // Corner is on high-res face && LOD > 2
                            if (lod > 2) {
                                isovalue1 = centerVoxel.isovalue;
                                cornerPos1 = centerVoxelPosition;
                            }
                        } else { // Else it's ok for LOD to be 2
                            isovalue1 = centerVoxel.isovalue;
                            cornerPos1 = centerVoxelPosition;
                        }
                    } else {
                        if (corner2 < 9) { // Corner is on high-res face && LOD > 2
                            if (lod > 2) {
                                isovalue2 = centerVoxel.isovalue;
                                cornerPos2 = centerVoxelPosition;
                            }
                        } else { // Else it's ok for LOD to be 2
                            isovalue2 = centerVoxel.isovalue;
                            cornerPos2 = centerVoxelPosition;
                        }
                    }
                }
                lod /= 2;
            }
        }
        // Scale the normal cell to be half the volume size along the transvoxel
        // axes
        // TODO: Implement the other cases
        if (axis == 1 << 5) { // Negative z axis
            if (corner1 >= 9) {
                cornerPos1.z += voxelScale * 0.5f;
            }
            if (corner2 >= 9) {
                cornerPos2.z += voxelScale * 0.5f;
            }
        }
        float t = isovalue2 / (isovalue2 - isovalue1);
        // std::cout << " T: " << t;
        glm::vec3 vertexPos = cornerPos1 * t + cornerPos2 * (1 - t);
        // std::cout << " Pos: (" << vertexPos[0] << "," << vertexPos[1] << "," <<
        // vertexPos[2] << ")" << std::endl;
        glm::vec3 normal = interpolateTransNormal(x, y, z, isorange, corner1, corner2);
        //        glm::vec3 delta(0, 0, voxelScale * 0.1f);
        //        int k = log2((float)voxelScale);
        //        glm::vec3 delta(0, 0, (1 - pow(2, -k) * x) * voxelScale/2);
        //        glm::mat3 transform(1-normal[0]*normal[0], -normal[0]*normal[1],
        //        -normal[0]*normal[2],
        //                            -normal[0]*normal[1], 1-normal[1]*normal[1],
        //                            -normal[1]*normal[2], -normal[0]*normal[2],
        //                            -normal[1]*normal[2], 1-normal[2]*normal[2]);
        //        if (corner1 >= 9 && corner2 >= 9)
        //            vertexPos += transform * delta;
        transEdgeVertices.push_back(vertexPos);
        transEdgeNormals.push_back(normal);
    }

    /* Create the triangles */
    if (transCellClass & 0x80) {
        // std::cout << "Rev Vertices: (";
        for (int i = transCellData.GetTriangleCount() * 3 - 1; i >= 0; i--) {
            int vertexIndex = transCellData.vertexIndex[i];
            // std::cout << vertexIndex << ",";
            for (int j = 0; j < 3; j++) {
                trianglesVector.push_back(transEdgeVertices[vertexIndex][j]);
                normalsVector.push_back(transEdgeNormals[vertexIndex][j]);
            }
        }
        // std::cout << ")" << std::endl;
    } else {
        // std::cout << "Vertices: (";
        for (int i = 0; i < transCellData.GetTriangleCount() * 3; i++) {
            int vertexIndex = transCellData.vertexIndex[i];
            // std::cout << vertexIndex << ",";
            for (int j = 0; j < 3; j++) {
                trianglesVector.push_back(transEdgeVertices[vertexIndex][j]);
                normalsVector.push_back(transEdgeNormals[vertexIndex][j]);
            }
        }
        // std::cout << ")" << std::endl;
    }
    //    for (auto vertexIndex : transCellData.vertexIndex) {
    //        for (int i = 0; i < 3; i++) {
    //            trianglesVector.push_back(transEdgeVertices[vertexIndex][i]);
    //            normalsVector.push_back(transEdgeNormals[vertexIndex][i]);
    //        }
    //    }
}

void TransvoxelSystem::setupVolumeTriangles()
{
    double time1 = glfwGetTime();
    trianglesVector.clear();
    normalsVector.clear();
    for (int x = voxelScale; x < size; x += voxelScale) {
        for (int y = voxelScale; y < size; y += voxelScale) {
            for (int z = voxelScale; z < size / 2 - voxelScale; z += voxelScale) {
                computeTrianglesForVoxel(x, y, z);
            }
        }
    }
    voxelScale *= 2;
    if (usingTransvoxels) {
        for (int x = voxelScale; x < size; x += voxelScale) {
            for (int y = voxelScale; y < size; y += voxelScale) {
                for (int z = size / 2 + voxelScale; z < size; z += voxelScale) {
                    computeTrianglesForVoxel(x, y, z);
                }
            }
        }
        for (int x = voxelScale; x < size; x += voxelScale) {
            for (int y = voxelScale; y < size; y += voxelScale) {
                for (int z = size / 2; z < size / 2 + voxelScale; z += voxelScale) {
                    computeTrianglesForTransvoxel(x, y, z);
                }
            }
        }
    } else {
        for (int x = voxelScale; x < size; x += voxelScale) {
            for (int y = voxelScale; y < size; y += voxelScale) {
                for (int z = size / 2; z < size; z += voxelScale) {
                    computeTrianglesForVoxel(x, y, z);
                }
            }
        }
    }
    voxelScale /= 2;

    double time2 = glfwGetTime();
    // std::cout << "Time to re-add all vertices to the vertex and normal buffers:
    // " << (time2 - time1) << std::endl;

    // Initialize the vertex array object
    glGenVertexArrays(1, &volumeVAO);
    glBindVertexArray(volumeVAO);

    // std::cout << "Triangle Vector Size: " << trianglesVector.size() / 3 <<
    // std::endl;
    //    std::cout << "Triangle Vector Size: " << scene.volumeData.size() * 30 <<
    //    std::endl;
    // generate vertex buffer to hand off to OGL
    glGenBuffers(1, &volumeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, volumeVBO);
    glBufferData(GL_ARRAY_BUFFER, trianglesVector.size() * sizeof(float),
        trianglesVector.data(), GL_DYNAMIC_DRAW);
    //    glBufferData(GL_ARRAY_BUFFER, scene.volumeData.size() * sizeof(Voxel),
    //    scene.volumeData.data(), GL_DYNAMIC_DRAW); glVertexAttribPointer(0, 3,
    //    GL_FLOAT, GL_FALSE, sizeof(Voxel), (GLvoid*)offsetof(Voxel, vertices));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
        (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // std::cout << "Normals Vector Size: " << normalsVector.size() / 3 <<
    // std::endl;
    //    std::cout << "Normals Vector Size: " << scene.volumeData.size() * 30 <<
    //    std::endl;
    glGenBuffers(1, &volumeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, volumeVBO);
    glBufferData(GL_ARRAY_BUFFER, normalsVector.size() * sizeof(float),
        normalsVector.data(), GL_DYNAMIC_DRAW);
    //    glBufferData(GL_ARRAY_BUFFER, scene.volumeData.size() * sizeof(Voxel),
    //    scene.volumeData.data(), GL_DYNAMIC_DRAW); glVertexAttribPointer(1, 3,
    //    GL_FLOAT, GL_FALSE, sizeof(Voxel), (GLvoid*)offsetof(Voxel, normals));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
        (GLvoid*)0);
    glEnableVertexAttribArray(1);
}

void TransvoxelSystem::setupVolumeData()
{
    setupVolumeIsovalues();
    setupVolumeTriangles();
}

void TransvoxelSystem::resetVolumeData()
{
    trianglesVector.clear();
    normalsVector.clear();
    setupVolumeData();
}

float TransvoxelSystem::getIsovalueFor3DSimplexNoise(int x, int y, int z)
{
    float scale = size;
    float isovalue = this->myModule.GetValue(x / scale, y / scale, z / scale);
    return isovalue;
}

void TransvoxelSystem::drawVolumeData(EngineSettings* engineSettings, float aspect)
{
    this->volumeShader->use();
    this->volumeShader->setMat4("P", engineSettings->projectionMatrix);
    this->volumeShader->setMat4("V", engineSettings->viewMatrix);

    this->modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f));
    this->modelMatrix = glm::translate(this->modelMatrix, glm::vec3(-size / 2.0f, -size / 2.0f, -size / 2.0f));
    this->modelMatrix = glm::translate(this->modelMatrix, glm::vec3(10, 10, -10));
    this->modelMatrix = glm::scale(this->modelMatrix, glm::vec3(0.5, 0.5, 0.5));
    this->volumeShader->setMat4("M", this->modelMatrix);

    glBindVertexArray(volumeVAO);
    glDrawArrays(GL_TRIANGLES, 0, trianglesVector.size());
    glBindVertexArray(0);
}
