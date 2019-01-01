#include "core/util.hpp"

// https://stackoverflow.com/questions/18022927/convert-high-resolution-clock-time-into-an-integer-chrono?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
// https://stackoverflow.com/questions/12937963/get-local-time-in-nanoseconds?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
// http://www.cplusplus.com/reference/thread/this_thread/sleep_for/
// https://github.com/bjnix/model-viewer

const char* readShaderFileToMemory(const char* filePath) {
  const char* shaderFileBuffer = NULL;

  std::ifstream inSdrFileStream(filePath);
  if (inSdrFileStream) {
    // Get length of shader file by seeking and telling (offset of 0)
    inSdrFileStream.seekg(0, inSdrFileStream.end);
    unsigned long fileLength = (unsigned long)inSdrFileStream.tellg() + 1;
    inSdrFileStream.seekg(0, inSdrFileStream.beg);

    // std::cout << "Shader File: Reading " << fileLength << " chars...\n";

    // Read shader file into a memory buffer (array)
    char* inputFileBuffer = new char[fileLength];
    memset(inputFileBuffer, 0, fileLength);
    inSdrFileStream.read(inputFileBuffer, fileLength);
    inputFileBuffer[fileLength - 1] = 0;

    // Close file and print status
    if (inputFileBuffer) {
      std::cout << "Read successfully: " << filePath << "\n";
      // std::cout << "---------------------------------\n";
      // std::cout << inputFileBuffer << std::endl;
      // std::cout << "---------------------------------\n";
      // std::cout << std::endl;
      inSdrFileStream.close();
    } else {
      std::cout << "... Error: Only " << inSdrFileStream.gcount()
                << " could be read!\n";
      inSdrFileStream.close();
      delete[] inputFileBuffer;
      return NULL;
    }

    // Hand over file contents to a const pointer
    shaderFileBuffer = inputFileBuffer;
    inputFileBuffer = NULL;
  } else {
    std::cout << "Shader File: Error. Not found!" << std::endl;
    return NULL;
  }

  return shaderFileBuffer;
}

double getTime() {
  return static_cast<double>(
             std::chrono::duration_cast<std::chrono::nanoseconds>(
                 std::chrono::high_resolution_clock::now().time_since_epoch())
                 .count()) /
         1000000000;
}

void sleep(int amount) {
  std::this_thread::sleep_for(std::chrono::milliseconds(amount));
}

/*int LoadGLTextures(const aiScene* scene)
{
        ILboolean success;

        ilInit();

        for (unsigned int m = 0; m<scene->mNumMaterials; ++m)
        {
                int texIndex = 0;
                aiString path;  // filename

                aiReturn texFound =
scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path); while
(texFound == AI_SUCCESS) {
                        //fill map with textures, OpenGL image ids set to 0
                        textureIdMap[path.data] = 0;
                        // more textures?
                        texIndex++;
                        texFound =
scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
                }
        }

        int numTextures = textureIdMap.size();

        ILuint* imageIds = new ILuint[numTextures];
        ilGenImages(numTextures, imageIds);

        GLuint* textureIds = new GLuint[numTextures];
        glGenTextures(numTextures, textureIds)

        std::map<std::string, GLuint>::iterator itr = textureIdMap.begin();
        int i = 0;
        for (; itr != textureIdMap.end(); ++i, ++itr)
        {
                //save IL image ID
                std::string filename = (*itr).first;  // get filename
                (*itr).second = textureIds[i];    // save texture id for
filename in map

                ilBindImage(imageIds[i]);
                ilEnable(IL_ORIGIN_SET);
                ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
                success = ilLoadImage((ILstring)filename.c_str());

                if (success) {
                        ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

                        glBindTexture(GL_TEXTURE_2D, textureIds[i]);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
GL_LINEAR); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA,
GL_UNSIGNED_BYTE, ilGetData());
                }
                else
                        printf("Couldn't load Image: %s\n", filename.c_str());
        }
        ilDeleteImages(numTextures, imageIds);

        //Cleanup
        delete[] imageIds;
        delete[] textureIds;

        //return success;
        return true;
}*/