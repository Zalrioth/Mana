#pragma once
#ifndef ORTHOGRAPHIC_CAMERA_H
#define ORTHOGRAPHIC_CAMERA_H

#include <ubermath/ubermath.h>

#include "mana//graphics/render/window.h"
#include "mana/graphics/graphicscommon.h"

#define YAW -90.0f
#define PITCH 0.0f
#define ROLL 90.0f
#define SPEED 2.5f
#define SENSITIVITY 0.1f
#define ZOOM 45.0f

enum OrthographicCameraMovement {
  ORTHO_FORWARD,
  ORTHO_BACKWARD,
  ORTHO_LEFT,
  ORTHO_RIGHT,
  ORTHO_UP,
  ORTHO_DOWN
};

struct OrthographicCamera {
  vec3 position;
  vec3 front;
  vec3 up;
  vec3 right;
  vec3 world_up;
  float yaw;
  float pitch;
  float roll;
  float sensitivity;
  float zoom;
  float z_near;
  float z_far;
  int mouse_locked;
  quat orientation;
  mat4 view;
};

void orthographic_camera_init(struct OrthographicCamera* orthographic_camera);
mat4 orthographic_camera_get_projection_matrix(struct OrthographicCamera* orthographic_camera, struct Window* window);
mat4 orthographic_camera_get_view_matrix(struct OrthographicCamera* orthographic_camera);
void orthographic_camera_update_vectors(struct OrthographicCamera* orthographic_camera);

#endif  // ORTHOGRAPHIC_CAMERA_H
