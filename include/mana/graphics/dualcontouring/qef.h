#pragma once
#ifndef QEF_H
#define QEF_H

#include <string.h>
#include <ubermath/ubermath.h>

struct QefData {
  float ata_00, ata_01, ata_02, ata_11, ata_12, ata_22;
  float atb_x, atb_y, atb_z;
  float btb;
  float mass_point_x, mass_point_y, mass_point_z;
  int num_points;
};

void qef_data_init(struct QefData *qef_data);
void qef_data_init_full(struct QefData *qef_data, const float ata_00, const float ata_01, const float ata_02, const float ata_11, const float ata_12, const float ata_22, const float atb_x, const float atb_y, const float atb_z, const float btb, const float massPoint_x, const float massPoint_y, const float massPoint_z, const int numPoints);
void qef_data_init_copy(struct QefData *qef_data, struct QefData *rhs);
void qef_data_add(struct QefData *qef_data, struct QefData *rhs);
void qef_data_clear(struct QefData *qef_data);
void qef_data_set(struct QefData *qef_data, const float ata_00, const float ata_01, const float ata_02, const float ata_11, const float ata_12, const float ata_22, const float atb_x, const float atb_y, const float atb_z, const float btb, const float mass_point_x, const float mass_point_y, const float mass_point_z, const int num_points);
void qef_data_set_copy(struct QefData *qef_data, struct QefData *rhs);

struct QefSolver {
  struct QefData data;
  mat3 ata;
  vec3 atb, mass_point, x;
  bool has_solution;
};

void qef_solver_init(struct QefSolver *qef_solver);
void qef_solver_add(struct QefSolver *qef_solver, const float px, const float py, const float pz, float nx, float ny, float nz);
void qef_solver_add_vec3(struct QefSolver *qef_solver, const vec3 p, const vec3 n);
void qef_solver_add_copy(struct QefSolver *qef_solver, struct QefData *rhs);
float qef_solver_get_error(struct QefSolver *qef_solver);
float qef_solver_get_error_pos(struct QefSolver *qef_solver, vec3 pos);
void qef_solver_reset(struct QefSolver *qef_solver);
void qef_solver_set_ata(struct QefSolver *qef_solver);
void qef_solver_set_atb(struct QefSolver *qef_solver);
float qef_solver_solve(struct QefSolver *qef_solver, vec3 outx, const float svd_tol, const int svd_sweeps, const float pinv_tol);

#endif  // QEF_H
