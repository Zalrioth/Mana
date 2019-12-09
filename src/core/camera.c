#include "core/camera.h"

void camera_init(struct Camera* camera) {
  memcpy(camera->position, (vec3){0.0f, 0.0f, 0.0f}, sizeof(vec3));
  memcpy(camera->front, (vec3){0.0f, 0.0f, -1.0f}, sizeof(vec3));
  memcpy(camera->up, (vec3){0.0f, 1.0f, 0.0f}, sizeof(vec3));
  memcpy(camera->world_up, camera->up, sizeof(vec3));
  memcpy(camera->right, (vec3){1.0f, 0.0f, 0.0f}, sizeof(vec3));
  camera->yaw = YAW;
  camera->pitch = PITCH;
  camera->zoom = ZOOM;
  camera->z_near = Z_NEAR;
  camera->z_far = Z_FAR;
  camera->sensitivity = SENSITIVITY;
  camera_update_vectors(camera);
}

void camera_get_projection_matrix(struct Camera* camera, struct Engine* engine, mat4* dest) {
  glm_perspective(glm_rad(camera->zoom), (float)engine->window.width / (float)engine->window.height, camera->z_near, camera->z_far, *dest);
}

void camera_get_view_matrix(struct Camera* camera, mat4* dest) {
  vec3 position_front;
  glm_vec3_add(camera->position, camera->front, position_front);
  glm_lookat(camera->position, position_front, camera->up, *dest);
}

void camera_update_vectors(struct Camera* camera) {
  camera->front[0] = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
  camera->front[1] = sin(glm_rad(camera->pitch));
  camera->front[2] = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
  glm_normalize(camera->front);
  glm_cross(camera->front, camera->world_up, camera->right);
  glm_normalize(camera->right);
  glm_cross(camera->right, camera->front, camera->up);
  glm_normalize(camera->up);
}
