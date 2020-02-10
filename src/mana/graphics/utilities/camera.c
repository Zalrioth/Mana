#include "mana/graphics/utilities/camera.h"

void camera_init(struct Camera* camera) {
  memcpy(camera->position, (vec3){0.0f, 0.0f, 0.0f}, sizeof(vec3));
  memcpy(camera->front, (vec3){0.0f, 0.0f, -1.0f}, sizeof(vec3));
  memcpy(camera->up, (vec3){0.0f, 1.0f, 0.0f}, sizeof(vec3));
  memcpy(camera->world_up, camera->up, sizeof(vec3));
  memcpy(camera->right, (vec3){1.0f, 0.0f, 0.0f}, sizeof(vec3));
  camera->yaw = YAW;
  camera->pitch = PITCH;
  camera->roll = ROLL;
  camera->zoom = ZOOM;
  camera->z_near = Z_NEAR;
  camera->z_far = Z_FAR;
  camera->sensitivity = SENSITIVITY;
  camera_update_vectors(camera);
}

void camera_get_projection_matrix(struct Camera* camera, struct Engine* engine, mat4 dest) {
  // The following will work but below implementation has inf Z far
  // Use the following if a restricted Z far is needed
  //glm_perspective(glm_rad(camera->zoom), (float)engine->window.width / (float)engine->window.height, camera->z_near, camera->z_far, dest);

  glm_mat4_zero(dest);

  float f = 1.0f / tan(glm_rad(camera->zoom) / 2.0f);
  dest[0][0] = f / ((float)engine->window.width / (float)engine->window.height);
  dest[1][1] = f;
  dest[2][3] = -1.0f;
  dest[3][2] = Z_NEAR;
}

void camera_get_view_matrix(struct Camera* camera, mat4 dest) {
  glm_mat4_zero(dest);

  vec3 position_front;
  glm_vec3_add(camera->position, camera->front, position_front);
  glm_lookat(camera->position, position_front, camera->up, dest);
}

void camera_update_vectors(struct Camera* camera) {
  vec3 direction = {0.0, 0.0, 0.0};
  direction[0] = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
  direction[1] = sin(glm_rad(camera->pitch));
  direction[2] = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
  glm_normalize(direction);
  glm_vec3_copy(direction, camera->front);

  camera->up[0] = cos(glm_rad(camera->roll));
  camera->up[1] = sin(glm_rad(camera->roll));
  glm_normalize(camera->up);

  glm_cross(camera->front, camera->up, camera->right);
  glm_normalize(camera->right);

  //glm_cross(camera->front, camera->world_up, camera->right);
  //glm_normalize(camera->right);
  //glm_cross(camera->right, camera->front, camera->up);
  //glm_normalize(camera->up);
}
