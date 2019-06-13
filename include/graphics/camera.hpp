#pragma once
#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <GL/glew.h> // This must appear before freeglut.h
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

//https://community.khronos.org/t/how-to-setup-projectionmatrix-worldmatrix-for-use-with-inverted-float-depth-buffer/73039/2
//https://www.reddit.com/r/opengl/comments/8z5egn/rendering_large_and_distant_object/

// Defines several possible options for camera movement. Used as abstraction to
// stay away from window-system specific input methods
enum Camera_Movement { FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN };

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const float Z_NEAR = 0.01f;
const float Z_FAR = 10000000.0f;

// An abstract camera class that processes input and calculates the
// corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera options
    float MouseSensitivity;
    float Zoom;
    // Z
    float ZNear;
    float ZFar;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float zNear = Z_NEAR, float zFar = Z_FAR);
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float zNear, float zFar);
    ~Camera();

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
};
#endif // CAMERA_HPP_