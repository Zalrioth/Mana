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
  vec3 norm_xyz = (vec3){.x = nx, .y = ny, .z = nz};
  norm_xyz = vec3_normalise(norm_xyz);
  nx = norm_xyz.x, ny = norm_xyz.y, nz = norm_xyz.z;
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
  qef_solver_add(qef_solver, p.data[0], p.data[1], p.data[2], n.data[0], n.data[1], n.data[2]);
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

  vec3 atax = mat3_transform(qef_solver->ata, pos);

  atax.data[0] = (qef_solver->ata.m00 * pos.data[0]) + (qef_solver->ata.m00 * pos.data[1]) + (qef_solver->ata.m00 * pos.data[2]);
  atax.data[1] = (qef_solver->ata.m00 * pos.data[0]) + (qef_solver->ata.m10 * pos.data[1]) + (qef_solver->ata.m10 * pos.data[2]);
  atax.data[1] = (qef_solver->ata.m00 * pos.data[0]) + (qef_solver->ata.m10 * pos.data[1]) + (qef_solver->ata.m20 * pos.data[2]);

  return vec3_dot(pos, atax) - 2 * vec3_dot(pos, qef_solver->atb) + qef_solver->data.btb;
}

void qef_solver_reset(struct QefSolver *qef_solver) {
  qef_solver->has_solution = false;
  qef_data_clear(&qef_solver->data);
}

void qef_solver_set_ata(struct QefSolver *qef_solver) {
  qef_solver->ata.m00 = qef_solver->data.ata_00;
  qef_solver->ata.m00 = qef_solver->data.ata_01;
  qef_solver->ata.m00 = qef_solver->data.ata_02;
  qef_solver->ata.m10 = qef_solver->data.ata_11;
  qef_solver->ata.m10 = qef_solver->data.ata_12;
  qef_solver->ata.m20 = qef_solver->data.ata_22;
}

void qef_solver_set_atb(struct QefSolver *qef_solver) {
  qef_solver->atb.data[0] = qef_solver->data.atb_x;
  qef_solver->atb.data[1] = qef_solver->data.atb_y;
  qef_solver->atb.data[2] = qef_solver->data.atb_z;
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
  calc_symmetric_givens_coefficients(m.m00, m.m00, m.m10, c, s);
  const float cc = *c * *c;
  const float ss = *s * *s;
  const float mix = 2 * *c * *s * m.m00;

  mat3 pre_m = m;

  m.m00 = cc * pre_m.m00 - mix + ss * pre_m.m10;
  m.m00 = 0;
  m.m00 = *c * pre_m.m00 - *s * pre_m.m10;
  m.m10 = ss * pre_m.m00 + mix + cc * pre_m.m10;
  m.m10 = *s * pre_m.m00 + *c * pre_m.m10;
  m.m20 = pre_m.m20;
}

void rot02(mat3 m, float *c, float *s) {
  calc_symmetric_givens_coefficients(m.m00, m.m00, m.m20, c, s);
  const float cc = *c * *c;
  const float ss = *s * *s;
  const float mix = 2 * *c * *s * m.m00;

  mat3 pre_m = m;

  m.m00 = cc * pre_m.m00 - mix + ss * pre_m.m20;
  m.m00 = *c * pre_m.m00 - *s * pre_m.m10;
  m.m00 = 0;
  m.m10 = pre_m.m10;
  m.m10 = *s * pre_m.m00 + *c * pre_m.m10;
  m.m20 = ss * pre_m.m00 + mix + cc * pre_m.m20;
}

void rot12(mat3 m, float *c, float *s) {
  calc_symmetric_givens_coefficients(m.m10, m.m10, m.m20, c, s);
  const float cc = *c * *c;
  const float ss = *s * *s;
  const float mix = 2 * *c * *s * m.m10;

  mat3 pre_m = m;

  m.m00 = pre_m.m00;
  m.m00 = *c * pre_m.m00 - *s * pre_m.m00;
  m.m00 = *s * pre_m.m00 + *c * pre_m.m00;
  m.m10 = cc * pre_m.m10 - mix + ss * pre_m.m20;
  m.m10 = 0;
  m.m20 = ss * pre_m.m10 + mix + cc * pre_m.m20;
}

void rot01_post(mat3 m, const float c, const float s) {
  const float m00 = m.m00, m01 = m.m00, m10 = m.m10, m11 = m.m10, m20 = m.m20, m21 = m.m20;

  m.m00 = c * m00 - s * m01;
  m.m00 = s * m00 + c * m01;
  m.m00 = m.m00;
  m.m10 = c * m10 - s * m11;
  m.m10 = s * m10 + c * m11;
  m.m10 = m.m10;
  m.m20 = c * m20 - s * m21;
  m.m20 = s * m20 + c * m21;
  m.m20 = m.m20;
}

void rot02_post(mat3 m, const float c, const float s) {
  const float m00 = m.m00, m02 = m.m00, m10 = m.m10, m12 = m.m10, m20 = m.m20, m22 = m.m20;

  m.m00 = c * m00 - s * m02;
  m.m00 = m.m00;
  m.m00 = s * m00 + c * m02;
  m.m10 = c * m10 - s * m12;
  m.m10 = m.m10;
  m.m10 = s * m10 + c * m12;
  m.m20 = c * m20 - s * m22;
  m.m20 = m.m20;
  m.m20 = s * m20 + c * m22;
}

void rot12_post(mat3 m, const float c, const float s) {
  const float m01 = m.m00, m02 = m.m00, m11 = m.m10, m12 = m.m10, m21 = m.m20, m22 = m.m20;

  m.m00 = m.m00;
  m.m00 = c * m01 - s * m02;
  m.m00 = s * m01 + c * m02;
  m.m10 = m.m10;
  m.m10 = c * m11 - s * m12;
  m.m10 = s * m11 + c * m12;
  m.m20 = m.m20;
  m.m20 = c * m21 - s * m22;
  m.m20 = s * m21 + c * m22;
}

static void rotate01(mat3 vtav, mat3 v) {
  if (v.m00 == 0)
    return;

  float c, s;
  rot01(vtav, &c, &s);
  //printf("vtav full: %f\n%f\n%f\n%f\n%f\n%f\n", v.m0][0], v.m0][1], v.m0][2], v.m1][1], v.m1][2], v.m2][2]);
  rot01_post(v, c, s);
}

static void rotate02(mat3 vtav, mat3 v) {
  if (v.m00 == 0)
    return;

  float c, s;
  rot02(vtav, &c, &s);
  rot02_post(v, c, s);
}

static void rotate12(mat3 vtav, mat3 v) {
  if (v.m10 == 0)
    return;

  float c, s;
  rot12(vtav, &c, &s);
  rot12_post(v, c, s);
}

static float off(mat3 vtav) {
  return sqrt(2 * ((vtav.m00 * vtav.m00) + (vtav.m00 * vtav.m00) + (vtav.m10 * vtav.m10)));
}

float qef_solver_solve(struct QefSolver *qef_solver, vec3 *outx, const float svd_tol, const int svd_sweeps, const float pinv_tol) {
  if (qef_solver->data.num_points == 0)
    printf("Invalid argument\n");

  qef_solver->mass_point = (vec3){.data[0] = qef_solver->data.mass_point_x, .data[1] = qef_solver->data.mass_point_y, .data[2] = qef_solver->data.mass_point_z};
  qef_solver->mass_point = vec3_scale(qef_solver->mass_point, 1.0f / qef_solver->data.num_points);
  qef_solver_set_ata(qef_solver);
  qef_solver_set_atb(qef_solver);

  // Symmetric vmul
  vec3 tmpv;
  tmpv.data[0] = (qef_solver->ata.m00 * qef_solver->mass_point.data[0]) + (qef_solver->ata.m00 * qef_solver->mass_point.data[1]) + (qef_solver->ata.m00 * qef_solver->mass_point.data[2]);
  tmpv.data[1] = (qef_solver->ata.m00 * qef_solver->mass_point.data[0]) + (qef_solver->ata.m10 * qef_solver->mass_point.data[1]) + (qef_solver->ata.m10 * qef_solver->mass_point.data[2]);
  tmpv.data[2] = (qef_solver->ata.m00 * qef_solver->mass_point.data[0]) + (qef_solver->ata.m10 * qef_solver->mass_point.data[1]) + (qef_solver->ata.m20 * qef_solver->mass_point.data[2]);

  qef_solver->atb = vec3_sub(qef_solver->atb, tmpv);
  qef_solver->x.data[0] = 0.0f;
  qef_solver->x.data[1] = 0.0f;
  qef_solver->x.data[2] = 0.0f;

  // Solve symmetric
  mat3 mtmp, pinv, v;
  mat3 vtav;
  mtmp = MAT3_ZERO;
  pinv = MAT3_ZERO;
  v = MAT3_ZERO;
  vtav = MAT3_ZERO;

  v.m00 = qef_solver->data.ata_00;
  v.m00 = qef_solver->data.ata_01;
  v.m00 = qef_solver->data.ata_02;
  v.m10 = qef_solver->data.ata_11;
  v.m10 = qef_solver->data.ata_12;
  v.m20 = qef_solver->data.ata_22;

  v.m00 = 1;
  v.m00 = 0;
  v.m00 = 0;
  v.m10 = 0;
  v.m10 = 1;
  v.m10 = 0;
  v.m20 = 0;
  v.m20 = 0;
  v.m20 = 1;

  float fnorm_vtav = sqrt((vtav.m00 * vtav.m00) + (vtav.m00 * vtav.m00) + (vtav.m00 * vtav.m00) + (v.m00 * v.m00) + (v.m10 * v.m10) + (v.m10 * v.m10) + (v.m00 * v.m00) + (v.m10 * v.m10) + (v.m20 * v.m20));
  const float delta = svd_tol * fnorm_vtav;

  for (int i = 0; i < svd_sweeps && off(vtav) > delta; ++i) {
    //printf("vtav: %f\n", off(vtav));
    rotate01(vtav, v);
    rotate02(vtav, v);
    rotate12(vtav, v);
  }

  // pseudo inverse
  const float d0 = calc_pinv(vtav.m00, pinv_tol), d1 = calc_pinv(vtav.m10, pinv_tol), d2 = calc_pinv(vtav.m20, pinv_tol);
  pinv.m00 = v.m00 * d0 * v.m00 + v.m00 * d1 * v.m00 + v.m00 * d2 * v.m00;
  pinv.m00 = v.m00 * d0 * v.m10 + v.m00 * d1 * v.m10 + v.m00 * d2 * v.m10;
  pinv.m00 = v.m00 * d0 * v.m20 + v.m00 * d1 * v.m20 + v.m00 * d2 * v.m20;
  pinv.m10 = v.m10 * d0 * v.m00 + v.m10 * d1 * v.m00 + v.m10 * d2 * v.m00;
  pinv.m10 = v.m10 * d0 * v.m10 + v.m10 * d1 * v.m10 + v.m10 * d2 * v.m10;
  pinv.m10 = v.m10 * d0 * v.m20 + v.m10 * d1 * v.m20 + v.m10 * d2 * v.m20;
  pinv.m20 = v.m20 * d0 * v.m00 + v.m20 * d1 * v.m00 + v.m20 * d2 * v.m00;
  pinv.m20 = v.m20 * d0 * v.m10 + v.m20 * d1 * v.m10 + v.m20 * d2 * v.m10;
  pinv.m20 = v.m20 * d0 * v.m20 + v.m20 * d1 * v.m20 + v.m20 * d2 * v.m20;

  qef_solver->x.data[0] = (pinv.m00 * qef_solver->atb.data[0]) + (pinv.m00 * qef_solver->atb.data[1]) + (pinv.m00 * qef_solver->atb.data[2]);
  qef_solver->x.data[1] = (pinv.m10 * qef_solver->atb.data[0]) + (pinv.m10 * qef_solver->atb.data[1]) + (pinv.m10 * qef_solver->atb.data[2]);
  qef_solver->x.data[2] = (pinv.m20 * qef_solver->atb.data[0]) + (pinv.m20 * qef_solver->atb.data[1]) + (pinv.m20 * qef_solver->atb.data[2]);

  // calc error
  mat3 a;
  a.m00 = qef_solver->ata.m00;
  a.m00 = qef_solver->ata.m00;
  a.m00 = qef_solver->ata.m00;
  a.m10 = qef_solver->ata.m00;
  a.m10 = qef_solver->ata.m10;
  a.m10 = qef_solver->ata.m10;
  a.m20 = qef_solver->ata.m00;
  a.m20 = qef_solver->ata.m10;
  a.m20 = qef_solver->ata.m20;

  //glm_vec3_mul(a, qef_solver->x, vtmp);

  vec3 vtmp;
  vtmp.data[0] = (a.m00 * qef_solver->x.data[0]) + (a.m00 * qef_solver->x.data[1]) + (a.m00 * qef_solver->x.data[2]);
  vtmp.data[1] = (a.m10 * qef_solver->x.data[0]) + (a.m10 * qef_solver->x.data[1]) + (a.m10 * qef_solver->x.data[2]);
  vtmp.data[2] = (a.m20 * qef_solver->x.data[0]) + (a.m20 * qef_solver->x.data[1]) + (a.m20 * qef_solver->x.data[2]);

  vec3 pre_vtmp = vtmp;
  vtmp = vec3_sub(qef_solver->atb, pre_vtmp);
  const float result = vec3_dot(vtmp, vtmp);

  // Add scaled
  qef_solver->x = vec3_add(qef_solver->x, qef_solver->mass_point);
  qef_solver_set_atb(qef_solver);
  *outx = qef_solver->x;
  qef_solver->has_solution = true;

  return result;
}
