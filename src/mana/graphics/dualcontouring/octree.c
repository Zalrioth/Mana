#include "mana/graphics/dualcontouring/octree.h"

const int MATERIAL_AIR = 0;
const int MATERIAL_SOLID = 1;

const float QEF_ERROR = 1e-6f;
const int QEF_SWEEPS = 4;

const ivec3 CHILD_MIN_OFFSETS[8] = {{0, 0, 0}, {0, 0, 1}, {0, 1, 0}, {0, 1, 1}, {1, 0, 0}, {1, 0, 1}, {1, 1, 0}, {1, 1, 1}};
const int edgevmap[12][2] = {{0, 4}, {1, 5}, {2, 6}, {3, 7}, {0, 2}, {1, 3}, {4, 6}, {5, 7}, {0, 1}, {2, 3}, {4, 5}, {6, 7}};
const int edgemask[3] = {5, 3, 6};
const int vert_map[8][3] = {{0, 0, 0}, {0, 0, 1}, {0, 1, 0}, {0, 1, 1}, {1, 0, 0}, {1, 0, 1}, {1, 1, 0}, {1, 1, 1}};
const int face_map[6][4] = {{4, 8, 5, 9}, {6, 10, 7, 11}, {0, 8, 1, 10}, {2, 9, 3, 11}, {0, 4, 2, 6}, {1, 5, 3, 7}};
const int cell_proc_face_mask[12][3] = {{0, 4, 0}, {1, 5, 0}, {2, 6, 0}, {3, 7, 0}, {0, 2, 1}, {4, 6, 1}, {1, 3, 1}, {5, 7, 1}, {0, 1, 2}, {2, 3, 2}, {4, 5, 2}, {6, 7, 2}};
const int cell_proc_edge_mask[6][5] = {{0, 1, 2, 3, 0}, {4, 5, 6, 7, 0}, {0, 4, 1, 5, 1}, {2, 6, 3, 7, 1}, {0, 2, 4, 6, 2}, {1, 3, 5, 7, 2}};
const int face_proc_face_mask[3][4][3] = {{{4, 0, 0}, {5, 1, 0}, {6, 2, 0}, {7, 3, 0}}, {{2, 0, 1}, {6, 4, 1}, {3, 1, 1}, {7, 5, 1}}, {{1, 0, 2}, {3, 2, 2}, {5, 4, 2}, {7, 6, 2}}};
const int face_proc_edge_mask[3][4][6] = {{{1, 4, 0, 5, 1, 1}, {1, 6, 2, 7, 3, 1}, {0, 4, 6, 0, 2, 2}, {0, 5, 7, 1, 3, 2}}, {{0, 2, 3, 0, 1, 0}, {0, 6, 7, 4, 5, 0}, {1, 2, 0, 6, 4, 2}, {1, 3, 1, 7, 5, 2}}, {{1, 1, 0, 3, 2, 0}, {1, 5, 4, 7, 6, 0}, {0, 1, 5, 0, 4, 1}, {0, 3, 7, 2, 6, 1}}};
const int edge_proc_edge_mask[3][2][5] = {{{3, 2, 1, 0, 0}, {7, 6, 5, 4, 0}}, {{5, 1, 4, 0, 1}, {7, 3, 6, 2, 1}}, {{6, 4, 2, 0, 2}, {7, 5, 3, 1, 2}}};
const int process_edge_mask[3][4] = {{3, 2, 1, 0}, {7, 5, 6, 4}, {11, 10, 9, 8}};

float length(vec3 pos) {
  return sqrtf((pos[0] * pos[0]) + (pos[1] * pos[1]) + (pos[2] * pos[2]));
}

float sphere_func(const vec3 worldPosition, const vec3 origin, float radius) {
  return length((vec3){worldPosition[0] - origin[0], worldPosition[1] - origin[1], worldPosition[2] - origin[2]}) - radius;
}

float cuboid_func(const vec3 worldPosition, const vec3 origin, const vec3 halfDimensions) {
  const vec3 local_pos = {worldPosition[0] - origin[0], worldPosition[1] - origin[1], worldPosition[2] - origin[2]};
  const vec3 pos = {local_pos[0], local_pos[1], local_pos[2]};

  vec3 d = {fabsf(pos[0]) - halfDimensions[0], fabsf(pos[1]) - halfDimensions[1], fabsf(pos[2]) - halfDimensions[2]};
  const float m = MAX(d[0], MAX(d[1], d[2]));

  d[0] = MAX(d[0], 0.0f);
  d[1] = MAX(d[1], 0.0f);
  d[2] = MAX(d[2], 0.0f);

  return MIN(m, length(d));
}

float density_func(float x_pos, float y_pos, float z_pos) {
  //float STEP = 64.0;
  //struct PerlinNoise perlin_noise = {0};
  //perlin_noise_init(&perlin_noise);
  //perlin_noise.parallel = false;
  //perlin_noise.octave_count = 4;
  //perlin_noise.frequency = 1.0;
  //perlin_noise.lacunarity = 2.2324f;
  //perlin_noise.persistence = 0.68324f;
  //perlin_noise.position[0] = x_pos / STEP;
  //perlin_noise.position[1] = y_pos / STEP;
  //perlin_noise.position[2] = z_pos / STEP;
  //
  //return perlin_noise_eval_3d_single(&perlin_noise);

  //float* noise_set = perlin_noise_eval_3d_sse2(&perlin_noise, 4, 4, 4);
  //float terrain = *noise_set;
  //noise_free(noise_set);
  //
  //return terrain;

  // Looks good
  float STEP = 64.0;
  struct RidgedFractalNoise ridged_fractal_noise = {0};
  ridged_fractal_noise_init(&ridged_fractal_noise);
  ridged_fractal_noise.octave_count = 4;
  ridged_fractal_noise.frequency = 1.0;
  ridged_fractal_noise.lacunarity = 2.2324f;
  ridged_fractal_noise.position[0] = x_pos / STEP;
  ridged_fractal_noise.position[1] = y_pos / STEP;
  ridged_fractal_noise.position[2] = z_pos / STEP;

  return ridged_fractal_noise_eval_3d_single(&ridged_fractal_noise);

  //float STEP = 16.0;
  //struct VoronoiNoise voronoi_noise = {0};
  //voronoi_noise_init(&voronoi_noise);
  ////voronoi_noise.octave_count = 4;
  //voronoi_noise.frequency = 1.0;
  ////voronoi_noise.displacement = 2.2324f;
  //voronoi_noise.position[0] = x_pos / STEP;
  //voronoi_noise.position[1] = y_pos / STEP;
  //voronoi_noise.position[2] = z_pos / STEP;
  //
  //return voronoi_noise_eval_3d_single(&voronoi_noise);

  //float STEP = 64.0;
  //struct BillowNoise billow_noise = {0};
  //billow_noise_init(&billow_noise);
  //billow_noise.parallel = false;
  //billow_noise.octave_count = 4;
  //billow_noise.frequency = 1.0;
  //billow_noise.lacunarity = 2.2324f;
  //billow_noise.persistence = 0.68324f;
  //billow_noise.position[0] = x_pos / STEP;
  //billow_noise.position[1] = y_pos / STEP;
  //billow_noise.position[2] = z_pos / STEP;
  //
  //return billow_noise_eval_3d_single(&billow_noise);

  //const float cube = cuboid_func((vec3){x_pos, y_pos, z_pos}, (vec3){-4., 10.f, -4.f}, (vec3){12.f, 12.f, 12.f});
  //const float sphere = sphere_func((vec3){x_pos, y_pos, z_pos}, (vec3){15.f, 2.5f, 1.f}, 16.f);
  //
  //return MIN(cube, -sphere);  //MAX(-cube, MIN(sphere, terrain));
}

void octree_draw_info_init(struct OctreeDrawInfo* octree_draw_info) {
  octree_draw_info->index = -1;
  octree_draw_info->corners = 0;
}

void octree_init(struct OctreeNode* octree_node, enum OctreeNodeType type) {
  octree_node->type = type;
  memset(octree_node->min, 0, sizeof(ivec3));
  octree_node->size = 0;
  octree_node->draw_info = NULL;
  for (int i = 0; i < 8; i++)
    octree_node->children[i] = NULL;
}

void octree_init_none(struct OctreeNode* octree_node) {
  octree_init(octree_node, NODE_NONE);
}

struct OctreeNode* octree_simplify_octree(struct OctreeNode* node, float threshold) {
  if (!node)
    return NULL;

  if (node->type != NODE_INTERNAL)
    return node;

  struct QefSolver qef = {0};
  qef_solver_init(&qef);

  int signs[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
  int midsign = -1;
  int edge_count = 0;
  bool is_collapsible = true;

  for (int i = 0; i < 8; i++) {
    node->children[i] = octree_simplify_octree(node->children[i], threshold);
    if (node->children[i]) {
      struct OctreeNode* child = node->children[i];
      if (child->type == NODE_INTERNAL)
        is_collapsible = false;
      else {
        qef_solver_add_copy(&qef, &child->draw_info->qef);

        midsign = (child->draw_info->corners >> (7 - i)) & 1;
        signs[i] = (child->draw_info->corners >> i) & 1;

        edge_count++;
      }
    }
  }

  if (!is_collapsible)
    return node;

  vec3 position = {0.0, 0.0, 0.0};
  qef_solver_solve(&qef, position, QEF_ERROR, QEF_SWEEPS, QEF_ERROR);
  float error = qef_solver_get_error(&qef);

  if (error > threshold)
    return node;

  if (position[0] < node->min[0] || position[0] > (node->min[0] + node->size) || position[1] < node->min[1] || position[1] > (node->min[1] + node->size) || position[2] < node->min[2] || position[2] > (node->min[2] + node->size))
    memcpy(position, qef.mass_point, sizeof(vec3));

  struct OctreeDrawInfo* draw_info = calloc(1, sizeof(struct OctreeDrawInfo));
  octree_draw_info_init(draw_info);

  for (int i = 0; i < 8; i++) {
    if (signs[i] == -1)
      draw_info->corners |= (midsign << i);
    else
      draw_info->corners |= (signs[i] << i);
  }

  memset(draw_info->average_normal, 0.0, sizeof(vec3));
  for (int i = 0; i < 8; i++) {
    if (node->children[i]) {
      struct OctreeNode* child = node->children[i];
      if (child->type == NODE_PSUEDO || child->type == NODE_LEAF)
        glm_vec3_add(draw_info->average_normal, child->draw_info->average_normal, draw_info->average_normal);
    }
  }

  glm_normalize(draw_info->average_normal);
  memcpy(draw_info->position, position, sizeof(vec3));
  memcpy(&draw_info->qef, &qef.data, sizeof(struct QefData));

  for (int i = 0; i < 8; i++) {
    octree_destroy_octree(node->children[i]);
    node->children[i] = NULL;
  }

  node->type = NODE_PSUEDO;
  node->draw_info = draw_info;

  return node;
}

void octree_generate_vertex_indices(struct OctreeNode* node, struct Mesh* mesh) {
  if (!node)
    return;

  if (node->type != NODE_LEAF)
    for (int i = 0; i < 8; i++)
      octree_generate_vertex_indices(node->children[i], mesh);

  if (node->type != NODE_INTERNAL) {
    struct OctreeDrawInfo* d = node->draw_info;
    if (!d) {
      printf("Error! Could not add vertex!\n");
      exit(EXIT_FAILURE);
    }

    d->index = vector_size(mesh->vertices);
    mesh_assign_vertex(mesh->vertices, d->position[0], d->position[1], d->position[2], d->average_normal[0], d->average_normal[1], d->average_normal[2], 0.0, 0.0);
  }
}

void octree_contour_process_edge(struct OctreeNode* node[4], int dir, struct Mesh* mesh) {
  int min_size = 1000000;
  int min_index = 0;
  int indices[4] = {-1, -1, -1, -1};
  bool flip = false;
  bool sign_change[4] = {false, false, false, false};

  for (int i = 0; i < 4; i++) {
    const int edge = process_edge_mask[dir][i];
    const int c1 = edgevmap[edge][0];
    const int c2 = edgevmap[edge][1];

    const int m1 = (node[i]->draw_info->corners >> c1) & 1;
    const int m2 = (node[i]->draw_info->corners >> c2) & 1;

    if (node[i]->size < min_size) {
      min_size = node[i]->size;
      min_index = i;
      flip = m1 != MATERIAL_AIR;
    }

    indices[i] = node[i]->draw_info->index;

    sign_change[i] = (m1 == MATERIAL_AIR && m2 != MATERIAL_AIR) || (m1 != MATERIAL_AIR && m2 == MATERIAL_AIR);
  }

  if (sign_change[min_index]) {
    if (!flip) {
      mesh_assign_indice(mesh->indices, indices[0]);
      mesh_assign_indice(mesh->indices, indices[1]);
      mesh_assign_indice(mesh->indices, indices[3]);

      mesh_assign_indice(mesh->indices, indices[0]);
      mesh_assign_indice(mesh->indices, indices[3]);
      mesh_assign_indice(mesh->indices, indices[2]);
    } else {
      mesh_assign_indice(mesh->indices, indices[0]);
      mesh_assign_indice(mesh->indices, indices[3]);
      mesh_assign_indice(mesh->indices, indices[1]);

      mesh_assign_indice(mesh->indices, indices[0]);
      mesh_assign_indice(mesh->indices, indices[2]);
      mesh_assign_indice(mesh->indices, indices[3]);
    }
  }
}

void octree_contour_edge_proc(struct OctreeNode* node[4], int dir, struct Mesh* mesh) {
  if (!node[0] || !node[1] || !node[2] || !node[3])
    return;

  if (node[0]->type != NODE_INTERNAL && node[1]->type != NODE_INTERNAL && node[2]->type != NODE_INTERNAL && node[3]->type != NODE_INTERNAL)
    octree_contour_process_edge(node, dir, mesh);
  else {
    for (int i = 0; i < 2; i++) {
      struct OctreeNode* edge_nodes[4];
      const int c[4] = {edge_proc_edge_mask[dir][i][0], edge_proc_edge_mask[dir][i][1], edge_proc_edge_mask[dir][i][2], edge_proc_edge_mask[dir][i][3]};

      for (int j = 0; j < 4; j++) {
        if (node[j]->type == NODE_LEAF || node[j]->type == NODE_PSUEDO)
          edge_nodes[j] = node[j];
        else
          edge_nodes[j] = node[j]->children[c[j]];
      }

      octree_contour_edge_proc(edge_nodes, edge_proc_edge_mask[dir][i][4], mesh);
    }
  }
}

void octree_contour_face_proc(struct OctreeNode* node[2], int dir, struct Mesh* mesh) {
  if (!node[0] || !node[1])
    return;

  if (node[0]->type == NODE_INTERNAL || node[1]->type == NODE_INTERNAL) {
    for (int i = 0; i < 4; i++) {
      struct OctreeNode* face_nodes[2];
      const int c[2] = {face_proc_face_mask[dir][i][0], face_proc_face_mask[dir][i][1]};

      for (int j = 0; j < 2; j++) {
        if (node[j]->type != NODE_INTERNAL)
          face_nodes[j] = node[j];
        else
          face_nodes[j] = node[j]->children[c[j]];
      }
      octree_contour_face_proc(face_nodes, face_proc_face_mask[dir][i][2], mesh);
    }

    const int orders[2][4] = {{0, 0, 1, 1}, {0, 1, 0, 1}};
    for (int i = 0; i < 4; i++) {
      struct OctreeNode* edgeNodes[4];
      const int c[4] = {face_proc_edge_mask[dir][i][1], face_proc_edge_mask[dir][i][2], face_proc_edge_mask[dir][i][3], face_proc_edge_mask[dir][i][4]};

      const int* order = orders[face_proc_edge_mask[dir][i][0]];
      for (int j = 0; j < 4; j++) {
        if (node[order[j]]->type == NODE_LEAF || node[order[j]]->type == NODE_PSUEDO)
          edgeNodes[j] = node[order[j]];
        else
          edgeNodes[j] = node[order[j]]->children[c[j]];
      }
      octree_contour_edge_proc(edgeNodes, face_proc_edge_mask[dir][i][5], mesh);
    }
  }
}

void octree_contour_cell_proc(struct OctreeNode* node, struct Mesh* mesh) {
  if (node == NULL)
    return;

  if (node->type == NODE_INTERNAL) {
    for (int i = 0; i < 8; i++)
      octree_contour_cell_proc(node->children[i], mesh);

    for (int i = 0; i < 12; i++) {
      struct OctreeNode* face_nodes[2];
      const int c[2] = {cell_proc_face_mask[i][0], cell_proc_face_mask[i][1]};

      face_nodes[0] = node->children[c[0]];
      face_nodes[1] = node->children[c[1]];

      octree_contour_face_proc(face_nodes, cell_proc_face_mask[i][2], mesh);
    }

    for (int i = 0; i < 6; i++) {
      struct OctreeNode* edge_nodes[4];
      const int c[4] = {cell_proc_edge_mask[i][0], cell_proc_edge_mask[i][1], cell_proc_edge_mask[i][2], cell_proc_edge_mask[i][3]};

      for (int j = 0; j < 4; j++)
        edge_nodes[j] = node->children[c[j]];

      octree_contour_edge_proc(edge_nodes, cell_proc_edge_mask[i][4], mesh);
    }
  }
}

void octree_approximate_zero_crossing_position(const vec3 p0, const vec3 p1, vec3 dest) {
  float min_value = 100000.0f;
  float t = 0.0f;
  float current_t = 0.0f;
  const int steps = 8;
  const float increment = 1.0f / (float)steps;

  while (current_t <= 1.0f) {
    const vec3 p = {p0[0] + ((p1[0] - p0[0]) * current_t), p0[1] + ((p1[1] - p0[1]) * current_t), p0[2] + ((p1[2] - p0[2]) * current_t)};
    const float density = fabsf(density_func(p[0], p[1], p[2]));
    if (density < min_value) {
      min_value = density;
      t = current_t;
    }

    current_t += increment;
  }

  dest[0] = p0[0] + ((p1[0] - p0[0]) * t);
  dest[1] = p0[1] + ((p1[1] - p0[1]) * t);
  dest[2] = p0[2] + ((p1[2] - p0[2]) * t);
}

void octree_calculate_surface_normal(const vec3 p, vec3 dest) {
  const float h = 0.001f;
  const float dx = density_func(p[0] + h, p[1], p[2]) - density_func(p[0] - h, p[1], p[2]);
  const float dy = density_func(p[0], p[1] + h, p[2]) - density_func(p[0], p[1] - h, p[2]);
  const float dz = density_func(p[0], p[1], p[2] + h) - density_func(p[0], p[1], p[2] - h);

  dest[0] = dx;
  dest[1] = dy;
  dest[2] = dz;
  glm_normalize(dest);
}

struct OctreeNode* octree_construct_leaf(struct OctreeNode* leaf) {
  if (!leaf || leaf->size != 1)
    return NULL;

  int corners = 0;
  for (int i = 0; i < 8; i++) {
    const ivec3 corner_pos = {leaf->min[0] + CHILD_MIN_OFFSETS[i][0], leaf->min[1] + CHILD_MIN_OFFSETS[i][1], leaf->min[2] + CHILD_MIN_OFFSETS[i][2]};
    // TODO: 3D array(noise data) -> land octree -> dual contouring octree
    // Note: Local chunks stick with 3D array for performance like grass growing
    const float density = density_func(corner_pos[0], corner_pos[1], corner_pos[2]);
    const int material = density < 0.0f ? MATERIAL_SOLID : MATERIAL_AIR;
    corners |= (material << i);
  }

  if (corners == 0 || corners == 255) {
    free(leaf);
    return NULL;
  }

  const int MAX_CROSSINGS = 6;
  int edge_count = 0;
  vec3 average_normal = {0.0, 0.0, 0.0};
  struct QefSolver qef = {0};
  qef_solver_init(&qef);

  for (int i = 0; i < 12 && edge_count < MAX_CROSSINGS; i++) {
    const int c1 = edgevmap[i][0];
    const int c2 = edgevmap[i][1];
    const int m1 = (corners >> c1) & 1;
    const int m2 = (corners >> c2) & 1;

    if ((m1 == MATERIAL_AIR && m2 == MATERIAL_AIR) || (m1 == MATERIAL_SOLID && m2 == MATERIAL_SOLID))
      continue;

    const vec3 p1 = {leaf->min[0] + CHILD_MIN_OFFSETS[c1][0], leaf->min[1] + CHILD_MIN_OFFSETS[c1][1], leaf->min[2] + CHILD_MIN_OFFSETS[c1][2]};
    const vec3 p2 = {leaf->min[0] + CHILD_MIN_OFFSETS[c2][0], leaf->min[1] + CHILD_MIN_OFFSETS[c2][1], leaf->min[2] + CHILD_MIN_OFFSETS[c2][2]};
    vec3 p = {0.0, 0.0, 0.0};
    octree_approximate_zero_crossing_position(p1, p2, p);
    vec3 n = {0.0, 0.0, 0.0};
    octree_calculate_surface_normal(p, n);
    qef_solver_add(&qef, p[0], p[1], p[2], n[0], n[1], n[2]);
    average_normal[0] = average_normal[0] + n[0];
    average_normal[1] = average_normal[1] + n[1];
    average_normal[2] = average_normal[2] + n[2];
    edge_count++;
  }

  vec3 qef_position = {0.0, 0.0, 0.0};
  qef_solver_solve(&qef, qef_position, QEF_ERROR, QEF_SWEEPS, QEF_ERROR);

  struct OctreeDrawInfo* draw_info = calloc(1, sizeof(struct OctreeDrawInfo));
  octree_draw_info_init(draw_info);

  memcpy(draw_info->position, qef_position, sizeof(vec3));
  memcpy(&draw_info->qef, &qef.data, sizeof(struct QefData));

  const vec3 min = {leaf->min[0], leaf->min[1], leaf->min[2]};
  const vec3 max = {leaf->min[0] + leaf->size, leaf->min[1] + leaf->size, leaf->min[2] + leaf->size};
  if (draw_info->position[0] < min[0] || draw_info->position[0] > max[0] || draw_info->position[1] < min[1] || draw_info->position[1] > max[1] || draw_info->position[2] < min[2] || draw_info->position[2] > max[2])
    memcpy(draw_info->position, qef.mass_point, sizeof(vec3));

  glm_vec3_divs(average_normal, (float)edge_count, average_normal);
  glm_normalize(average_normal);
  memcpy(draw_info->average_normal, average_normal, sizeof(vec3));
  draw_info->corners = corners;

  leaf->type = NODE_LEAF;
  leaf->draw_info = draw_info;

  return leaf;
}

struct OctreeNode* octree_construct_octree_nodes(struct OctreeNode* node) {
  if (!node)
    return NULL;

  if (node->size == 1)
    return octree_construct_leaf(node);

  const int child_size = node->size / 2;
  bool has_children = false;

  for (int i = 0; i < 8; i++) {
    struct OctreeNode* child = calloc(1, sizeof(struct OctreeNode));
    octree_init_none(child);
    child->size = child_size;
    child->min[0] = node->min[0] + (CHILD_MIN_OFFSETS[i][0] * child_size);
    child->min[1] = node->min[1] + (CHILD_MIN_OFFSETS[i][1] * child_size);
    child->min[2] = node->min[2] + (CHILD_MIN_OFFSETS[i][2] * child_size);
    child->type = NODE_INTERNAL;

    node->children[i] = octree_construct_octree_nodes(child);
    has_children |= (node->children[i] != NULL);
  }

  if (!has_children) {
    free(node);
    return NULL;
  }

  return node;
}

struct OctreeNode* octree_build_octree(const ivec3 min, const int size, const float threshold) {
  struct OctreeNode* root = calloc(1, sizeof(struct OctreeNode));
  octree_init_none(root);
  memcpy(root->min, min, sizeof(ivec3));
  root->size = size;
  root->type = NODE_INTERNAL;

  octree_construct_octree_nodes(root);
  // TODO: Fix this
  //root = octree_simplify_octree(root, threshold);

  return root;
}

void octree_generate_mesh_from_octree(struct OctreeNode* node, struct Mesh* mesh) {
  if (!node)
    return;

  mesh_clear(mesh);

  octree_generate_vertex_indices(node, mesh);
  octree_contour_cell_proc(node, mesh);
}

void octree_destroy_octree(struct OctreeNode* node) {
  if (!node)
    return;

  for (int i = 0; i < 8; i++)
    octree_destroy_octree(node->children[i]);

  if (node->draw_info)
    free(node->draw_info);

  free(node);
}
