#include "mana/graphics/dualcontouring/qef.h"

void qef_data_init(struct QefData *qef_data) {
  qef_data_clear(qef_data);
}

void qef_data_init_full(struct QefData *qef_data, const float ata_00, const float ata_01, const float ata_02, const float ata_11, const float ata_12, const float ata_22, const float atb_x, const float atb_y, const float atb_z, const float btb, const float massPoint_x, const float massPoint_y, const float massPoint_z, const int numPoints) {
  qef_data_set(qef_data, ata_00, ata_01, ata_02, ata_11, ata_12, ata_22, atb_x, atb_y, atb_z, btb, massPoint_x, massPoint_y, massPoint_z, numPoints);
}

void qef_data_init_copy(struct QefData *qef_data, struct QefData *rhs) {
  qef_data_set_copy(qef_data, rhs);
}

void qef_data_add(struct QefData *qef_data, struct QefData *rhs) {
  qef_data->ata_00 += rhs->ata_00;
  qef_data->ata_01 += rhs->ata_01;
  qef_data->ata_02 += rhs->ata_02;
  qef_data->ata_11 += rhs->ata_11;
  qef_data->ata_12 += rhs->ata_12;
  qef_data->ata_22 += rhs->ata_22;
  qef_data->atb_x += rhs->atb_x;
  qef_data->atb_y += rhs->atb_y;
  qef_data->atb_z += rhs->atb_z;
  qef_data->btb += rhs->btb;
  qef_data->mass_point_x += rhs->mass_point_x;
  qef_data->mass_point_y += rhs->mass_point_y;
  qef_data->mass_point_z += rhs->mass_point_z;
  qef_data->num_points += rhs->num_points;
}

void qef_data_clear(struct QefData *qef_data) {
  memset(qef_data, 0, sizeof(struct QefData));
}

void qef_data_set(struct QefData *qef_data, const float ata_00, const float ata_01, const float ata_02, const float ata_11, const float ata_12, const float ata_22, const float atb_x, const float atb_y, const float atb_z, const float btb, const float mass_point_x, const float mass_point_y, const float mass_point_z, const int num_points) {
  qef_data->ata_00 = ata_00;
  qef_data->ata_01 = ata_01;
  qef_data->ata_02 = ata_02;
  qef_data->ata_11 = ata_11;
  qef_data->ata_12 = ata_12;
  qef_data->ata_22 = ata_22;
  qef_data->atb_x = atb_x;
  qef_data->atb_y = atb_y;
  qef_data->atb_z = atb_z;
  qef_data->btb = btb;
  qef_data->mass_point_x = mass_point_x;
  qef_data->mass_point_y = mass_point_y;
  qef_data->mass_point_z = mass_point_z;
  qef_data->num_points = num_points;
}

void qef_data_set_copy(struct QefData *qef_data, struct QefData *rhs) {
  qef_data_set(qef_data, rhs->ata_00, rhs->ata_01, rhs->ata_02, rhs->ata_11, rhs->ata_12, rhs->ata_22, rhs->atb_x, rhs->atb_y, rhs->atb_z, rhs->btb, rhs->mass_point_x, rhs->mass_point_y, rhs->mass_point_z, rhs->num_points);
}

void qef_solver_init(struct QefSolver *qef_solver) {
  qef_solver->has_solution = false;
}

void qef_solver_add(struct QefSolver *qef_solver, const float px, const float py, const float pz, float nx, float ny, float nz) {
  qef_solver->has_solution = false;
  vec3 norm_xyz = {nx, ny, nz};
  glm_normalize(norm_xyz);
  nx = norm_xyz[0], ny = norm_xyz[1], nz = norm_xyz[2];
  qef_solver->data.ata_00 += nx * nx;
  qef_solver->data.ata_01 += nx * ny;
  qef_solver->data.ata_02 += nx * nz;
  qef_solver->data.ata_11 += ny * ny;
  qef_solver->data.ata_12 += ny * nz;
  qef_solver->data.ata_22 += nz * nz;
  const float dot = nx * px + ny * py + nz * pz;
  qef_solver->data.atb_x += dot * nx;
  qef_solver->data.atb_y += dot * ny;
  qef_solver->data.atb_z += dot * nz;
  qef_solver->data.btb += dot * dot;
  qef_solver->data.mass_point_x += px;
  qef_solver->data.mass_point_y += py;
  qef_solver->data.mass_point_z += pz;
  ++qef_solver->data.num_points;
}

void qef_solver_add_vec3(struct QefSolver *qef_solver, const vec3 p, const vec3 n) {
  qef_solver_add(qef_solver, p[0], p[1], p[2], n[0], n[1], n[2]);
}

void qef_solver_add_copy(struct QefSolver *qef_solver, struct QefData *rhs) {
  qef_solver->has_solution = false;
  qef_data_add(&qef_solver->data, rhs);
}

float qef_solver_get_error(struct QefSolver *qef_solver) {
  if (!qef_solver->has_solution)
    printf("Illegal state\n");

  return qef_solver_get_error_pos(qef_solver, qef_solver->x);
}

float qef_solver_get_error_pos(struct QefSolver *qef_solver, vec3 pos) {
  if (!qef_solver->has_solution) {
    qef_solver_set_ata(qef_solver);
    qef_solver_set_atb(qef_solver);
  }

  vec3 atax;
  glm_mat3_mulv(qef_solver->ata, pos, atax);

  atax[0] = (qef_solver->ata[0][0] * pos[0]) + (qef_solver->ata[0][1] * pos[1]) + (qef_solver->ata[0][2] * pos[2]);
  atax[1] = (qef_solver->ata[0][1] * pos[0]) + (qef_solver->ata[1][1] * pos[1]) + (qef_solver->ata[1][2] * pos[2]);
  atax[1] = (qef_solver->ata[0][2] * pos[0]) + (qef_solver->ata[1][2] * pos[1]) + (qef_solver->ata[2][2] * pos[2]);

  return glm_vec3_dot(pos, atax) - 2 * glm_vec3_dot(pos, qef_solver->atb) + qef_solver->data.btb;
}

void qef_solver_reset(struct QefSolver *qef_solver) {
  qef_solver->has_solution = false;
  qef_data_clear(&qef_solver->data);
}

void qef_solver_set_ata(struct QefSolver *qef_solver) {
  qef_solver->ata[0][0] = qef_solver->data.ata_00;
  qef_solver->ata[0][1] = qef_solver->data.ata_01;
  qef_solver->ata[0][2] = qef_solver->data.ata_02;
  qef_solver->ata[1][1] = qef_solver->data.ata_11;
  qef_solver->ata[1][2] = qef_solver->data.ata_12;
  qef_solver->ata[2][2] = qef_solver->data.ata_22;
}

void qef_solver_set_atb(struct QefSolver *qef_solver) {
  qef_solver->atb[0] = qef_solver->data.atb_x;
  qef_solver->atb[1] = qef_solver->data.atb_y;
  qef_solver->atb[2] = qef_solver->data.atb_z;
}

static float calc_pinv(const float x, const float tol) {
  return (fabs(x) < tol || fabs(1 / x) < tol) ? 0 : (1 / x);
}

static void calc_symmetric_givens_coefficients(const float a_pp, const float a_pq, const float a_qq, float *c, float *s) {
  if (a_pq == 0) {
    *c = 1.0;
    *s = 0.0;
    return;
  }

  const float tau = (a_qq - a_pp) / (2 * a_pq);
  const float stt = sqrt(1.0f + tau * tau);
  const float tan = 1.0f / ((tau >= 0) ? (tau + stt) : (tau - stt));
  *c = 1.0f / sqrt(1.0f + tan * tan);
  *s = tan * (*c);
}

void rot01(mat3 m, float *c, float *s) {
  //printf("vtav full: %f\n%f\n%f\n%f\n%f\n%f\n", m[0][0], m[0][1], m[0][2], m[1][1], m[1][2], m[2][2]);
  calc_symmetric_givens_coefficients(m[0][0], m[0][1], m[1][1], c, s);
  const float cc = *c * *c;
  const float ss = *s * *s;
  const float mix = 2 * *c * *s * m[0][1];

  mat3 pre_m;
  glm_mat3_copy(m, pre_m);

  m[0][0] = cc * pre_m[0][0] - mix + ss * pre_m[1][1];
  m[0][1] = 0;
  m[0][2] = *c * pre_m[0][2] - *s * pre_m[1][2];
  m[1][1] = ss * pre_m[0][0] + mix + cc * pre_m[1][1];
  m[1][2] = *s * pre_m[0][2] + *c * pre_m[1][2];
  m[2][2] = pre_m[2][2];
}

void rot02(mat3 m, float *c, float *s) {
  calc_symmetric_givens_coefficients(m[0][0], m[0][2], m[2][2], c, s);
  const float cc = *c * *c;
  const float ss = *s * *s;
  const float mix = 2 * *c * *s * m[0][2];

  mat3 pre_m;
  glm_mat3_copy(m, pre_m);

  m[0][0] = cc * pre_m[0][0] - mix + ss * pre_m[2][2];
  m[0][1] = *c * pre_m[0][1] - *s * pre_m[1][2];
  m[0][2] = 0;
  m[1][1] = pre_m[1][1];
  m[1][2] = *s * pre_m[0][1] + *c * pre_m[1][2];
  m[2][2] = ss * pre_m[0][0] + mix + cc * pre_m[2][2];
}

void rot12(mat3 m, float *c, float *s) {
  calc_symmetric_givens_coefficients(m[1][1], m[1][2], m[2][2], c, s);
  const float cc = *c * *c;
  const float ss = *s * *s;
  const float mix = 2 * *c * *s * m[1][2];

  mat3 pre_m;
  glm_mat3_copy(m, pre_m);

  m[0][0] = pre_m[0][0];
  m[0][1] = *c * pre_m[0][1] - *s * pre_m[0][2];
  m[0][2] = *s * pre_m[0][1] + *c * pre_m[0][2];
  m[1][1] = cc * pre_m[1][1] - mix + ss * pre_m[2][2];
  m[1][2] = 0;
  m[2][2] = ss * pre_m[1][1] + mix + cc * pre_m[2][2];
}

void rot01_post(mat3 m, const float c, const float s) {
  const float m00 = m[0][0], m01 = m[0][1], m10 = m[1][0], m11 = m[1][1], m20 = m[2][0], m21 = m[2][1];

  m[0][0] = c * m00 - s * m01;
  m[0][1] = s * m00 + c * m01;
  m[0][2] = m[0][2];
  m[1][0] = c * m10 - s * m11;
  m[1][1] = s * m10 + c * m11;
  m[1][2] = m[1][2];
  m[2][0] = c * m20 - s * m21;
  m[2][1] = s * m20 + c * m21;
  m[2][2] = m[2][2];
}

void rot02_post(mat3 m, const float c, const float s) {
  const float m00 = m[0][0], m02 = m[0][2], m10 = m[1][0], m12 = m[1][2], m20 = m[2][0], m22 = m[2][2];

  m[0][0] = c * m00 - s * m02;
  m[0][1] = m[0][1];
  m[0][2] = s * m00 + c * m02;
  m[1][0] = c * m10 - s * m12;
  m[1][1] = m[1][1];
  m[1][2] = s * m10 + c * m12;
  m[2][0] = c * m20 - s * m22;
  m[2][1] = m[2][1];
  m[2][2] = s * m20 + c * m22;
}

void rot12_post(mat3 m, const float c, const float s) {
  const float m01 = m[0][1], m02 = m[0][2], m11 = m[1][1], m12 = m[1][2], m21 = m[2][1], m22 = m[2][2];

  m[0][0] = m[0][0];
  m[0][1] = c * m01 - s * m02;
  m[0][2] = s * m01 + c * m02;
  m[1][0] = m[1][0];
  m[1][1] = c * m11 - s * m12;
  m[1][2] = s * m11 + c * m12;
  m[2][0] = m[2][0];
  m[2][1] = c * m21 - s * m22;
  m[2][2] = s * m21 + c * m22;
}

static void rotate01(mat3 vtav, mat3 v) {
  if (vtav[0][1] == 0)
    return;

  float c, s;
  rot01(vtav, &c, &s);
  //printf("vtav full: %f\n%f\n%f\n%f\n%f\n%f\n", vtav[0][0], vtav[0][1], vtav[0][2], vtav[1][1], vtav[1][2], vtav[2][2]);
  rot01_post(v, c, s);
}

static void rotate02(mat3 vtav, mat3 v) {
  if (vtav[0][2] == 0)
    return;

  float c, s;
  rot02(vtav, &c, &s);
  rot02_post(v, c, s);
}

static void rotate12(mat3 vtav, mat3 v) {
  if (vtav[1][2] == 0)
    return;

  float c, s;
  rot12(vtav, &c, &s);
  rot12_post(v, c, s);
}

static float off(mat3 vtav) {
  return sqrt(2 * ((vtav[0][1] * vtav[0][1]) + (vtav[0][2] * vtav[0][2]) + (vtav[1][2] * vtav[1][2])));
}

float qef_solver_solve(struct QefSolver *qef_solver, vec3 outx, const float svd_tol, const int svd_sweeps, const float pinv_tol) {
  if (qef_solver->data.num_points == 0)
    printf("Invalid argument\n");

  memcpy(qef_solver->mass_point, (vec3){qef_solver->data.mass_point_x, qef_solver->data.mass_point_y, qef_solver->data.mass_point_z}, sizeof(vec3));
  glm_vec3_scale(qef_solver->mass_point, 1.0f / qef_solver->data.num_points, qef_solver->mass_point);
  qef_solver_set_ata(qef_solver);
  qef_solver_set_atb(qef_solver);

  // Symmetric vmul
  vec3 tmpv = {0.0, 0.0, 0.0};
  tmpv[0] = (qef_solver->ata[0][0] * qef_solver->mass_point[0]) + (qef_solver->ata[0][1] * qef_solver->mass_point[1]) + (qef_solver->ata[0][2] * qef_solver->mass_point[2]);
  tmpv[1] = (qef_solver->ata[0][1] * qef_solver->mass_point[0]) + (qef_solver->ata[1][1] * qef_solver->mass_point[1]) + (qef_solver->ata[1][2] * qef_solver->mass_point[2]);
  tmpv[2] = (qef_solver->ata[0][2] * qef_solver->mass_point[0]) + (qef_solver->ata[1][2] * qef_solver->mass_point[1]) + (qef_solver->ata[2][2] * qef_solver->mass_point[2]);

  glm_vec3_sub(qef_solver->atb, tmpv, qef_solver->atb);
  qef_solver->x[0] = 0.0f;
  qef_solver->x[1] = 0.0f;
  qef_solver->x[2] = 0.0f;

  // Solve symmetric
  mat3 mtmp, pinv, v;
  mat3 vtav;
  glm_mat3_zero(mtmp);
  glm_mat3_zero(pinv);
  glm_mat3_zero(v);
  glm_mat3_zero(vtav);

  vtav[0][0] = qef_solver->data.ata_00;
  vtav[0][1] = qef_solver->data.ata_01;
  vtav[0][2] = qef_solver->data.ata_02;
  vtav[1][1] = qef_solver->data.ata_11;
  vtav[1][2] = qef_solver->data.ata_12;
  vtav[2][2] = qef_solver->data.ata_22;

  v[0][0] = 1;
  v[0][1] = 0;
  v[0][2] = 0;
  v[1][0] = 0;
  v[1][1] = 1;
  v[1][2] = 0;
  v[2][0] = 0;
  v[2][1] = 0;
  v[2][2] = 1;

  float fnorm_vtav = sqrt((vtav[0][0] * vtav[0][0]) + (vtav[0][1] * vtav[0][1]) + (vtav[0][2] * vtav[0][2]) + (vtav[0][1] * vtav[0][1]) + (vtav[1][1] * vtav[1][1]) + (vtav[1][2] * vtav[1][2]) + (vtav[0][2] * vtav[0][2]) + (vtav[1][2] * vtav[1][2]) + (vtav[2][2] * vtav[2][2]));
  const float delta = svd_tol * fnorm_vtav;

  for (int i = 0; i < svd_sweeps && off(vtav) > delta; ++i) {
    //printf("vtav: %f\n", off(vtav));
    rotate01(vtav, v);
    rotate02(vtav, v);
    rotate12(vtav, v);
  }

  // pseudo inverse
  const float d0 = calc_pinv(vtav[0][0], pinv_tol), d1 = calc_pinv(vtav[1][1], pinv_tol), d2 = calc_pinv(vtav[2][2], pinv_tol);
  pinv[0][0] = v[0][0] * d0 * v[0][0] + v[0][1] * d1 * v[0][1] + v[0][2] * d2 * v[0][2];
  pinv[0][1] = v[0][0] * d0 * v[1][0] + v[0][1] * d1 * v[1][1] + v[0][2] * d2 * v[1][2];
  pinv[0][2] = v[0][0] * d0 * v[2][0] + v[0][1] * d1 * v[2][1] + v[0][2] * d2 * v[2][2];
  pinv[1][0] = v[1][0] * d0 * v[0][0] + v[1][1] * d1 * v[0][1] + v[1][2] * d2 * v[0][2];
  pinv[1][1] = v[1][0] * d0 * v[1][0] + v[1][1] * d1 * v[1][1] + v[1][2] * d2 * v[1][2];
  pinv[1][2] = v[1][0] * d0 * v[2][0] + v[1][1] * d1 * v[2][1] + v[1][2] * d2 * v[2][2];
  pinv[2][0] = v[2][0] * d0 * v[0][0] + v[2][1] * d1 * v[0][1] + v[2][2] * d2 * v[0][2];
  pinv[2][1] = v[2][0] * d0 * v[1][0] + v[2][1] * d1 * v[1][1] + v[2][2] * d2 * v[1][2];
  pinv[2][2] = v[2][0] * d0 * v[2][0] + v[2][1] * d1 * v[2][1] + v[2][2] * d2 * v[2][2];

  qef_solver->x[0] = (pinv[0][0] * qef_solver->atb[0]) + (pinv[0][1] * qef_solver->atb[1]) + (pinv[0][2] * qef_solver->atb[2]);
  qef_solver->x[1] = (pinv[1][0] * qef_solver->atb[0]) + (pinv[1][1] * qef_solver->atb[1]) + (pinv[1][2] * qef_solver->atb[2]);
  qef_solver->x[2] = (pinv[2][0] * qef_solver->atb[0]) + (pinv[2][1] * qef_solver->atb[1]) + (pinv[2][2] * qef_solver->atb[2]);

  // calc error
  mat3 a;
  glm_mat3_zero(a);
  vec3 vtmp = {0.0, 0.0, 0.0};
  a[0][0] = qef_solver->ata[0][0];
  a[0][1] = qef_solver->ata[0][1];
  a[0][2] = qef_solver->ata[0][2];
  a[1][0] = qef_solver->ata[0][1];
  a[1][1] = qef_solver->ata[1][1];
  a[1][2] = qef_solver->ata[1][2];
  a[2][0] = qef_solver->ata[0][2];
  a[2][1] = qef_solver->ata[1][2];
  a[2][2] = qef_solver->ata[2][2];

  //glm_vec3_mul(a, qef_solver->x, vtmp);

  vtmp[0] = (a[0][0] * qef_solver->x[0]) + (a[0][1] * qef_solver->x[1]) + (a[0][2] * qef_solver->x[2]);
  vtmp[1] = (a[1][0] * qef_solver->x[0]) + (a[1][1] * qef_solver->x[1]) + (a[1][2] * qef_solver->x[2]);
  vtmp[2] = (a[2][0] * qef_solver->x[0]) + (a[2][1] * qef_solver->x[1]) + (a[2][2] * qef_solver->x[2]);

  vec3 pre_vtmp;
  glm_vec3_copy(vtmp, pre_vtmp);

  glm_vec3_sub(qef_solver->atb, pre_vtmp, vtmp);
  const float result = glm_vec3_dot(vtmp, vtmp);

  // Add scaled
  glm_vec3_add(qef_solver->x, qef_solver->mass_point, qef_solver->x);
  qef_solver_set_atb(qef_solver);
  memcpy(outx, qef_solver->x, sizeof(vec3));
  qef_solver->has_solution = true;

  return result;
}
