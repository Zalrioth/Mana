#include "graphics/camera.h"

int createCamera(vec3 position)
{
    Front = vec3(0.0f, 0.0f, -1.0f);
    Position = vec3(posX, posY, posZ);
    WorldUp = vec3(0.0f, 1.0f, 0.0f);
    yaw = YAW;
    pitch = PITCH;
    Zoom = ZOOM;
    MouseSensitivity = SENSITIVITY;
    updateCameraVectors();

    return 0;
}

mat4 GetViewMatrix()
{
    return lookAt(Position, Position + Front, Up);
}

void updateCameraVectors()
{
    vec3 front;
    front.x = cos(radians(Yaw)) * cos(radians(Pitch));
    front.y = sin(radians(Pitch));
    front.z = sin(radians(Yaw)) * cos(radians(Pitch));
    Front = normalize(front);
    Right = normalize(cross(Front, WorldUp));
    Up = normalize(cross(Right, Front));
}
