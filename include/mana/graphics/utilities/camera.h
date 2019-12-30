#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>
#include "mana/core/engine.h"

#define YAW -90.0f
#define PITCH 0.0f
#define SPEED 2.5f
#define SENSITIVITY 0.1f
#define ZOOM 45.0f
#define Z_NEAR 0.01f
#define Z_FAR 1000.0f

enum CameraMovement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  UP,
  DOWN
};

struct Camera {
  vec3 position;
  vec3 front;
  vec3 up;
  vec3 right;
  vec3 world_up;
  float yaw;
  float pitch;
  float sensitivity;
  float zoom;
  float z_near;
  float z_far;
};

void camera_init(struct Camera* camera);
void camera_get_projection_matrix(struct Camera* camera, struct Engine* engine, mat4* dest);
void camera_get_view_matrix(struct Camera* camera, mat4* dest);
void camera_update_vectors(struct Camera* camera);

#endif  // CAMERA_H