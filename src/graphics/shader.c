#include "graphics/shader.h"

int read_file(char* buffer, const int bufferSize, const char* fileName)
{
    FILE* fp;

    fp = fopen(fileName, "r");
    if (fp == NULL) {
        printf("Could not open file %s", fileName);
        return errno;
    }
    while (fgets(buffer, bufferSize, fp) != NULL)
        printf("%s", buffer);
    fclose(fp);

    return 0;
}

int create_shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    char vertexCode[SHADER_SIZE];
    char fragmentCode[SHADER_SIZE];
    char geometryCode[SHADER_SIZE];
    int readFileError;

    if ((readFileError = read_file(vertexCode, SHADER_SIZE, vertexPath)) != 0)
        return readFileError;

    if ((readFileError = read_file(fragmentCode, SHADER_SIZE, fragmentPath)) != 0)
        return readFileError;

    if (geometryPath != NULL) {
        if ((readFileError = read_file(geometryCode, SHADER_SIZE, geometryPath)) != 0)
            return readFileError;
    }

    const char* vShaderCode = vertexCode;
    const char* fShaderCode = fragmentCode;
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCocheck_compile_errorsmpilcheck_compile_errorseErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    check_compile_errors(fragment, "FRAGMENT");

    unsigned int geometry;
    if (geometryPath != NULL) {
        const char* gShaderCode = geometryCode;
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        check_compile_errors(geometry, "GEOMETRY");
    }

    unsigned int ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (geometryPath != NULL)
        glAttachShader(ID, geometry);
    glLinkProgram(ID);
    check_compile_errors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath != NULL)
        glDeleteShader(geometry);

    return ID;
}

void use(int ID)
{
    glUseProgram(ID);
}
void set_bool(int ID, const char* name, bool value)
{
    glUniform1i(glGetUniformLocation(ID, name), (int)value);
}
void set_int(int ID, const char* name, int value)
{
    glUniform1i(glGetUniformLocation(ID, name), value);
}
void setFloat(int ID, const char* name, float value)
{
    glUniform1f(glGetUniformLocation(ID, name), value);
}
void set_vec2(int ID, const char* name, const vec2 value)
{
    glUniform2fv(glGetUniformLocation(ID, name), 1, value);
}
/*void set_vec2_raw(int ID, const char* name, float x, float y)
{
    glUniform2f(glGetUniformLocation(ID, name), x, y);
}*/
void set_vec3(int ID, const char* name, const vec3 value)
{
    glUniform3fv(glGetUniformLocation(ID, name), 1, value);
}
void set_vec3_raw(int ID, const char* name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}
void set_vec4(int ID, const char* name, const vec4 value)
{
    glUniform4fv(glGetUniformLocation(ID, name), 1, value);
}
void set_vec4_raw(int ID, const char* name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
}
/*void set_mat2(int ID, const char* name, const mat2 mat)
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name), 1, GL_FALSE, (float*)mat);
}*/
void set_mat3(int ID, const char* name, const mat3 mat)
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name), 1, GL_FALSE, (float*)mat);
}
void set_mat4(int ID, const char* name, const mat4 mat)
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, (float*)mat);
}

void check_compile_errors(GLuint shader, const char* type)
{
    GLint success;
    GLchar infoLog[1024];
    if (strcmp(type, "PROGRAM")) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            printf("ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\n -- --------------------------------------------------- -- \n", type, infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            printf("ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s\n -- --------------------------------------------------- -- \n", type, infoLog);
        }
    }
}