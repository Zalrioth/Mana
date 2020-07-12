#include "mana/graphics/utilities/camera.h"

void camera_init(struct Camera* camera) {
  camera->position = VEC3_ZERO;
  camera->front = (vec3){.x = 0.0f, .y = 0.0f, .z = -1.0f};
  camera->up = (vec3){.x = 0.0f, .y = 1.0f, .z = 0.0f};
  camera->world_up = camera->up;
  camera->right = (vec3){.x = 1.0f, .y = 0.0f, .z = 0.0f};
  camera->yaw = YAW;
  camera->pitch = PITCH;
  camera->roll = ROLL;
  camera->zoom = ZOOM;
  camera->z_near = Z_NEAR;
  camera->z_far = Z_FAR;
  camera->sensitivity = SENSITIVITY;
  camera_update_vectors(camera);
}

mat4 camera_get_projection_matrix(struct Camera* camera, struct Window* window) {
  float f = 1.0f / tan(degree_to_radian(camera->zoom) / 2.0f);
  mat4 dest = MAT4_ZERO;
  dest.vecs[0].data[0] = f / ((float)window->width / (float)window->height);
  dest.vecs[1].data[1] = f;
  dest.vecs[2].data[3] = -1.0f;
  dest.vecs[3].data[2] = Z_NEAR;
  return dest;
}

mat4 camera_get_view_matrix(struct Camera* camera) {
  vec3 position_front = vec3_add(camera->position, camera->front);
  return mat4_look_at(camera->position, position_front, camera->up);
}

void camera_update_vectors(struct Camera* camera) {
  vec3 direction = (vec3){.x = 0.0, .y = 0.0, .z = 0.0};
  direction.data[0] = cos(degree_to_radian(camera->yaw)) * cos(degree_to_radian(camera->pitch));
  direction.data[1] = sin(degree_to_radian(camera->pitch));
  direction.data[2] = sin(degree_to_radian(camera->yaw)) * cos(degree_to_radian(camera->pitch));
  direction = vec3_normalise(direction);
  camera->front = direction;

  camera->up.data[0] = cos(degree_to_radian(camera->roll));
  camera->up.data[1] = sin(degree_to_radian(camera->roll));
  camera->up = vec3_normalise(camera->up);

  camera->right = vec3_cross_product(camera->front, camera->up);
  camera->right = vec3_normalise(camera->right);

  //glm_cross(camera->front, camera->world_up, camera->right);
  //glm_normalize(camera->right);
  //glm_cross(camera->right, camera->front, camera->up);
  //glm_normalize(camera->up);
}
