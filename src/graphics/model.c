#include "graphics/model.h"

int DoTheImportThing(char* pFile)
{
    // Start the import on the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll t
    // probably to request more postprocessing than we do in this example.
    const struct aiScene* scene = aiImportFile(pFile, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
    // If the import failed, report it
    if (!scene) {
        //DoTheErrorLogging(aiGetErrorString());
        return 1;
    }
    // Now we can access the file's contents
    //DoTheSceneProcessing(scene);
    // We're done. Release all resources associated with this import
    aiReleaseImport(scene);
    return 0;
}