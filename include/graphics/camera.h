#pragma once
#ifndef CAMERA_H_
#define CAMERA_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

enum Camera_Movement { FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN };

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

vec3 Position;
vec3 Front;
vec3 Up;
vec3 Right;
vec3 WorldUp;
float Yaw;
float Pitch;
float MouseSensitivity;
float Zoom;

int createCamera(vec3 position);
mat4* GetViewMatrix();
void updateCameraVectors();
#endif // CAMERA_H_