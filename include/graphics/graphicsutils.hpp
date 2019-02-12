#pragma once
#ifndef GRAPHICS_UTILS_HPP_
#define GRAPHICS_UTILS_HPP_

#define GLEW_STATIC
#include <GL/glew.h> // This must appear before freeglut.h
#include <GLFW/glfw3.h>
#include <stdlib.h>

GLuint createTexture(int width, int height);
GLuint createTexture16f(int width, int height);
GLuint createFloatingTexture(int width, int height);
GLuint createDepthTexture(int width, int height);
GLuint createFBO();
void attachTextureNum(int fbo, int texture, int num);
void attachDepthTexture(int fbo, int texture);
float generateFloat();

#endif // GRAPHICS_UTILS_HPP_