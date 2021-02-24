#include "mana/graphics/utilities/orthographiccamera.h"

void orthographic_camera_init(struct OrthographicCamera* orthographic_camera) {
  orthographic_camera->position = VEC3_ZERO;
  orthographic_camera->front = (vec3){.x = 0.0f, .y = 0.0f, .z = -1.0f};
  orthographic_camera->world_up = (vec3){.x = 0.0f, .y = 1.0f, .z = 0.0f};
  orthographic_camera->up = orthographic_camera->world_up;
  orthographic_camera->right = (vec3){.x = 1.0f, .y = 0.0f, .z = 0.0f};
  orthographic_camera->yaw = YAW;
  orthographic_camera->pitch = PITCH;
  orthographic_camera->roll = ROLL;
  orthographic_camera->orientation = QUAT_DEFAULT;
  orthographic_camera->zoom = ZOOM;
  orthographic_camera->z_near = Z_NEAR;
  orthographic_camera->z_far = Z_FAR;
  orthographic_camera->sensitivity = SENSITIVITY;
  orthographic_camera_update_vectors(orthographic_camera);
}

mat4 orthographic_camera_get_projection_matrix(struct OrthographicCamera* orthographic_camera, struct Window* window) {
  //float f = 1.0f / tan(degree_to_radian(orthographic_camera->zoom) / 2.0f);
  //mat4 dest = MAT4_ZERO;
  //dest.vecs[0].data[0] = f / ((float)window->width / (float)window->height);
  //dest.vecs[1].data[1] = f;
  //dest.vecs[2].data[3] = -1.0f;
  //dest.vecs[3].data[2] = Z_NEAR;
  //return dest;

  float rl, tb, fn;

  mat4 dest = MAT4_ZERO;

  float right = 1.0;
  float left = -1.0;
  float top = 1.0;
  float bottom = -1.0;

  rl = 1.0f / (right - left);
  tb = 1.0f / (top - bottom);
  fn = -1.0f / (Z_FAR - Z_NEAR);

  dest.vecs[0].data[0] = 2.0f * rl;
  dest.vecs[1].data[1] = 2.0f * tb;
  dest.vecs[2].data[2] = 2.0f * fn;
  dest.vecs[3].data[0] = -(right + left) * rl;
  dest.vecs[3].data[1] = -(top + bottom) * tb;
  dest.vecs[3].data[2] = (Z_FAR + Z_NEAR) * fn;
  dest.vecs[3].data[3] = 1.0f;

  return dest;
}

mat4 orthographic_camera_get_view_matrix(struct OrthographicCamera* orthographic_camera) {
  return mat4_look_at(orthographic_camera->position, vec3_add(orthographic_camera->position, orthographic_camera->front), orthographic_camera->up);
  //return orthographic_camera->view;
}

//static inline vec3 qrot(quat q, vec3 v) {
//  return v + 2.0 * vec3_cross_product(quaternion_to_vec3(q), vec3_cross_product(quaternion_to_vec3(q), v) + q.w * v);
//}

void orthographic_camera_update_vectors(struct OrthographicCamera* orthographic_camera) {
  orthographic_camera->front = vec3_normalise((vec3){.data[0] = cos(degree_to_radian(orthographic_camera->yaw)) * cos(degree_to_radian(orthographic_camera->pitch)), .data[1] = sin(degree_to_radian(orthographic_camera->pitch)), .data[2] = sin(degree_to_radian(orthographic_camera->yaw)) * cos(degree_to_radian(orthographic_camera->pitch))});
  orthographic_camera->right = vec3_normalise(vec3_cross_product(orthographic_camera->front, orthographic_camera->world_up));
  orthographic_camera->up = vec3_normalise(vec3_cross_product(orthographic_camera->right, orthographic_camera->front));
}
