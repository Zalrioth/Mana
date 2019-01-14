#pragma once
#ifndef SHADER_H_
#define SHADER_H_

#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/types.h>

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>

#define SHADER_SIZE 4096

#include "graphics/shader.h"

int read_file(const char* buffer, const int bufferSize, const char* fileName);
int create_shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);
void use(int ID);
void set_bool(int ID, const char* name, bool value);
void set_int(int ID, const char* name, int value);
void setFloat(int ID, const char* name, float value);
//void set_vec2(int ID, const char* name, const vec2 value);
void set_vec2_raw(int ID, const char* name, float x, float y);
void set_vec3(int ID, const char* name, const vec3 value);
void set_vec3_raw(int ID, const char* name, float x, float y, float z);
void set_vec4(int ID, const char* name, const vec4 value);
void set_vec4_raw(int ID, const char* name, float x, float y, float z, float w);
//void set_mat2(int ID, const char* name, const mat2 mat);
void set_mat3(int ID, const char* name, const mat3 mat);
void set_mat4(int ID, const char* name, const mat4 mat);
void check_compile_errors(GLuint shader, const char* type);

#endif // SHADER_H_