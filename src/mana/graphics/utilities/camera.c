#include "mana/graphics/utilities/camera.h"

void camera_init(struct Camera* camera) {
  camera->position = VEC3_ZERO;
  camera->front = (vec3){.x = 0.0f, .y = 0.0f, .z = -1.0f};
  camera->world_up = (vec3){.x = 0.0f, .y = 1.0f, .z = 0.0f};
  camera->up = camera->world_up;
  camera->right = (vec3){.x = 1.0f, .y = 0.0f, .z = 0.0f};
  camera->yaw = YAW;
  camera->pitch = PITCH;
  camera->roll = ROLL;
  //camera->prev_yaw = camera->yaw;
  //camera->prev_pitch = camera->pitch;
  //camera->prev_roll = camera->roll;
  camera->orientation = QUAT_DEFAULT;
  camera->zoom = ZOOM;
  camera->z_near = Z_NEAR;
  camera->z_far = Z_FAR;
  camera->sensitivity = SENSITIVITY;
  //camera->camera_quat = QUAT_DEFAULT;
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
  return mat4_look_at(camera->position, vec3_add(camera->position, camera->front), camera->up);
  //return camera->view;
}

//static inline vec3 qrot(quat q, vec3 v) {
//  return v + 2.0 * vec3_cross_product(quaternion_to_vec3(q), vec3_cross_product(quaternion_to_vec3(q), v) + q.w * v);
//}

void camera_update_vectors(struct Camera* camera) {
  //camera->world_up = (vec3){.x = cosf(camera->position.x), .y = cosf(camera->position.y), .z = cosf(camera->position.z)};

  // Free quaternion camera
  //quat key_quat = (quat){.data[0] = degree_to_radian(camera->pitch), .data[1] = degree_to_radian(camera->yaw), .data[2] = degree_to_radian(camera->roll), .data[3] = 1.0f};
  //camera->pitch = camera->yaw = camera->roll = 0.0f;
  //camera->orientation = quaternion_normalise(quaternion_mul(key_quat, camera->orientation));
  //mat4 rotate = quaternion_to_mat4_other(camera->orientation);
  //mat4 translate = mat4_translate(MAT4_IDENTITY, vec3_scale(camera->position, -1.0f));
  //camera->view = mat4_mul(rotate, translate);

  // Free euler camera
  ////Gimbal problems
  //// Pitch
  //if (fabs(camera->pitch - camera->prev_pitch) > 0.001f) {
  //  camera->front = vec3_normalise(vec3_add(vec3_scale(camera->front, cos(degree_to_radian(camera->pitch))), vec3_scale(camera->up, sin(degree_to_radian(camera->pitch)))));
  //  camera->up = vec3_normalise(vec3_cross_product(camera->front, camera->right));
  //  camera->up = vec3_scale(camera->up, -1.0f);
  //  camera->pitch = camera->prev_pitch = 0.0f;
  //}
  //
  //// Yaw
  //if (fabs(camera->yaw - camera->prev_yaw) > 0.001f) {
  //  camera->front = vec3_normalise(vec3_add(vec3_scale(camera->front, cos(degree_to_radian(camera->yaw))), vec3_scale(camera->right, sin(degree_to_radian(camera->yaw)))));
  //  camera->right = vec3_normalise(vec3_cross_product(camera->front, camera->up));
  //  camera->yaw = camera->prev_yaw = 0.0f;
  //}
  //
  //// Roll
  //if (fabs(camera->roll - camera->prev_roll) > 0.001f) {
  //  camera->right = vec3_normalise(vec3_add(vec3_scale(camera->right, cos(degree_to_radian(camera->roll))), vec3_scale(camera->up, sin(degree_to_radian(camera->roll)))));
  //  camera->up = vec3_normalise(vec3_cross_product(camera->front, camera->right));
  //  camera->up = vec3_scale(camera->up, -1.0f);
  //  camera->roll = camera->prev_roll = 0.0f;
  //}

  // FPS euler camera
  camera->front = vec3_normalise((vec3){.data[0] = cos(degree_to_radian(camera->yaw)) * cos(degree_to_radian(camera->pitch)), .data[1] = sin(degree_to_radian(camera->pitch)), .data[2] = sin(degree_to_radian(camera->yaw)) * cos(degree_to_radian(camera->pitch))});
  camera->right = vec3_normalise(vec3_cross_product(camera->front, camera->world_up));
  camera->up = vec3_normalise(vec3_cross_product(camera->right, camera->front));

  // FPS euler camera
  //camera->front = vec3_normalise((vec3){.data[0] = cos(degree_to_radian(camera->yaw)) * cos(degree_to_radian(camera->pitch)), .data[1] = sin(degree_to_radian(camera->pitch)), .data[2] = sin(degree_to_radian(camera->yaw)) * cos(degree_to_radian(camera->pitch))});
  //camera->right = vec3_normalise(vec3_cross_product(camera->front, camera->world_up));
  //camera->up = vec3_normalise(vec3_cross_product(camera->right, camera->front));
}
