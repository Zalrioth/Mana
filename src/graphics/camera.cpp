#include "graphics/camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float zNear, float zFar)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f))
    , Zoom(ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    ZNear = zNear;
    ZFar = zFar;
    MouseSensitivity = SENSITIVITY;
    updateCameraVectors();
}
// Constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float zNear, float zFar)
    : Camera(glm::vec3(posX, posY, posZ), glm::vec3(upX, upY, upZ), yaw, pitch, zNear, ZFar)
{
}

Camera::~Camera() {}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}
// Calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(
        Front, WorldUp)); // Normalize the vectors, because their length gets
    // closer to 0 the more you look up or down which
    // results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}
