#include "mana/graphics/dualcontouring/manifold/manifoldoctree.h"

static inline bool manifold_octree_construct_nodes(struct ManifoldOctreeNode* octree_node, int threads);
static inline bool manifold_octree_construct_leaf(struct ManifoldOctreeNode* octree_node);

void manifold_octree_construct_base(struct ManifoldOctreeNode* octree_node, int size, float error) {
  octree_node->index = 0;
  octree_node->position = VEC3_ZERO;
  octree_node->size = size;
  octree_node->type = MANIFOLD_NODE_INTERNAL;
  octree_node->child_index = 0;
  manifold_octree_construct_nodes(octree_node, omp_get_max_threads());
}

static inline void manifold_octree_find_vertice(struct Vertex* vertex, struct Map* vertice_map) {
  if (vertex == NULL)
    return;

  manifold_octree_find_vertice(vertex->parent, vertice_map);

  // NOTE: On a 64 bit archetiecture 8 bytes are allocated for a memory address, yet a hex value uses 4 bits per value. Meaning a memory address in hex is length of 16 numbers aka 8 bytes * 8 bits / 4 hex
  char buffer[((sizeof(char*) * 8) / 4) + 1] = {0};
  sprintf(buffer, "%p", vertex);
  map_set(vertice_map, buffer, &vertex);
}

void manifold_octree_destroy_octree(struct ManifoldOctreeNode* octree_node, struct Map* vertice_map) {
  if (!octree_node)
    return;

  for (int i = 0; i < 8; i++)
    manifold_octree_destroy_octree(octree_node->children[i], vertice_map);

  if (octree_node->vertices) {
    for (int vertice_num = 0; vertice_num < array_list_size(octree_node->vertices); vertice_num++)
      manifold_octree_find_vertice(array_list_get(octree_node->vertices, vertice_num), vertice_map);
    array_list_delete(octree_node->vertices);
    free(octree_node->vertices);
  }

  free(octree_node);
}

struct VertexFound {
  struct VertexManifoldDualContouring vertex_data;
  struct Vertex* vertex_handle;
};

static inline void manifold_octree_generate_vertex_buffer_thread_split(struct ManifoldOctreeNode* octree_node, struct Vector* vertices) {
  if (octree_node->type != MANIFOLD_NODE_LEAF) {
    for (int i = 0; i < 8; i++) {
      if (octree_node->children[i] != NULL)
        manifold_octree_generate_vertex_buffer_thread_split(octree_node->children[i], vertices);
    }
  }

  if (vertices == NULL || octree_node->vertices == NULL || array_list_size(octree_node->vertices) == 0)
    return;

  for (int i = 0; i < array_list_size(octree_node->vertices); i++) {
    struct Vertex* ver = (struct Vertex*)array_list_get(octree_node->vertices, i);
    if (ver == NULL)
      continue;
    vec3 nc = vec3_old_skool_normalise(vec3_add(vec3_scale(ver->normal, 0.5f), vec3_scale(VEC3_ONE, 0.5f)));
    vec3 solved_x = VEC3_ZERO;
    qef_solver_solve(&ver->qef, &solved_x, 1e-6f, 4, 1e-6f);
    vector_push_back(vertices, (struct VertexFound[]){(struct VertexFound){.vertex_data = (struct VertexManifoldDualContouring){.position.x = solved_x.x, .position.y = solved_x.y, .position.z = solved_x.z, .color.r = nc.r, .color.g = nc.g, .color.b = nc.b, .normal1.r = ver->normal.r, .normal1.g = ver->normal.g, .normal1.b = ver->normal.b, .normal2.r = ver->normal.r, .normal2.g = ver->normal.g, .normal2.b = ver->normal.b}, .vertex_handle = ver}});
  }
}

void manifold_octree_generate_vertex_buffer(struct ManifoldOctreeNode* octree_node, struct Vector* vertices) {
  struct Vector vert_vectors[8] = {0};
  for (int vector_num = 0; vector_num < 8; vector_num++)
    vector_init(&vert_vectors[vector_num], sizeof(struct VertexFound));

  // Spawn thread for each octree child
  if (octree_node->type != MANIFOLD_NODE_LEAF) {
#pragma omp parallel for num_threads(omp_get_max_threads())
    for (int i = 0; i < 8; i++) {
      if (octree_node->children[i] != NULL)
        manifold_octree_generate_vertex_buffer_thread_split(octree_node->children[i], &vert_vectors[i]);
    }
  }

  // Combine child vectors
  for (int vector_num = 0; vector_num < 8; vector_num++) {
    for (int vert_num = 0; vert_num < vector_size(&vert_vectors[vector_num]); vert_num++) {
      struct VertexFound* new_vertex = vector_get(&vert_vectors[vector_num], vert_num);
      new_vertex->vertex_handle->index = vector_size(vertices);
      mesh_manifold_dual_contouring_assign_vertex_simple(vertices, new_vertex->vertex_data);
    }
  }

  for (int vector_num = 0; vector_num < 8; vector_num++)
    vector_delete(&vert_vectors[vector_num]);
}

// TODO: Optimize below as much as possible

static inline bool manifold_octree_construct_nodes(struct ManifoldOctreeNode* octree_node, int threads) {
  if (octree_node->size == 1)
    return manifold_octree_construct_leaf(octree_node);

  octree_node->type = MANIFOLD_NODE_INTERNAL;
  int child_size = octree_node->size / 2;
  bool has_children = false;

#pragma omp parallel for num_threads(threads) if (threads > 0)
  for (int index = 0; index < 8; index++) {
    vec3 child_pos = TCornerDeltas[index];
    struct ManifoldOctreeNode* new_node = calloc(1, sizeof(struct ManifoldOctreeNode));
    octree_node_init(new_node, vec3_add(octree_node->position, vec3_scale(child_pos, (float)child_size)), child_size, MANIFOLD_NODE_INTERNAL);
    octree_node->children[index] = new_node;
    octree_node->children[index]->child_index = index;
    // NOTE: If user has cpu with very high thread count could technically go one more level would be threads - (8 ^ depth)
    if (manifold_octree_construct_nodes(octree_node->children[index], 0))
      has_children |= true;
    else {
      octree_node->children[index] = NULL;
      free(new_node);
    }
  }

  return has_children;
}

static inline bool manifold_octree_construct_leaf(struct ManifoldOctreeNode* octree_node) {
  if (octree_node->size != 1)
    return false;

  octree_node->type = MANIFOLD_NODE_LEAF;
  int corners = 0;
  float samples[8] = {0};

  // TODO: Will need to keep this for ifdef non avx/sse
  //for (int i = 0; i < 8; i++) {
  //  if ((samples[i] = Sphere(vec3_add(octree_node->position, TCornerDeltas[i]))) < 0)
  //    corners |= 1 << i;
  //}

  float poss[8][3] = {0};
  for (int i = 0; i < 8; i++) {
    vec3 npos = vec3_add(octree_node->position, TCornerDeltas[i]);
    memcpy(poss[i], &npos, sizeof(vec3));
  }
  SphereSamples(poss, samples);
  for (int i = 0; i < 8; i++) {
    if (samples[i] < 0)
      corners |= 1 << i;
  }

  octree_node->corners = (unsigned char)corners;

  if (corners == 0 || corners == 255)
    return false;

  int total_edges = TransformedVerticesNumberTable[octree_node->corners];
  int v_edges[4][16] = {0};

  int v_index = 0;
  int e_index = 0;
  memset(v_edges[0], -1, sizeof(int) * 8);
  for (int e = 0; e < 16; e++) {
    int code = TransformedEdgesTable[corners][e];
    if (code == -2) {
      v_index++;
      break;
    }
    if (code == -1) {
      v_index++;
      e_index = 0;
      memset(v_edges[v_index], -1, sizeof(int) * 13);
      continue;
    }

    v_edges[v_index][e_index++] = code;
  }

  octree_node->vertices = calloc(1, sizeof(struct ArrayList));
  array_list_init(octree_node->vertices);

  for (int i = 0; i < v_index; i++) {
    int k = 0;
    struct Vertex* get_vertice = calloc(1, sizeof(struct Vertex));
    vertex_init(get_vertice);
    array_list_add(octree_node->vertices, get_vertice);
    qef_solver_init(&get_vertice->qef);
    vec3 normal = VEC3_ZERO;
    int ei[12] = {0};
    while (v_edges[i][k] != -1) {
      ei[v_edges[i][k]] = 1;
      vec3 a = vec3_add(octree_node->position, vec3_scale(TCornerDeltas[TEdgePairs[v_edges[i][k]][0]], octree_node->size));
      vec3 b = vec3_add(octree_node->position, vec3_scale(TCornerDeltas[TEdgePairs[v_edges[i][k]][1]], octree_node->size));
      vec3 intersection = GetIntersection(a, b, samples[TEdgePairs[v_edges[i][k]][0]], samples[TEdgePairs[v_edges[i][k]][1]]);
      vec3 n = GetNormal(intersection);
      normal = vec3_add(normal, n);
      qef_solver_add_simp(&get_vertice->qef, intersection, n);
      k++;
    }

    normal = vec3_old_skool_divs(normal, k);
    normal = vec3_old_skool_normalise(normal);
    get_vertice->index = array_list_size(octree_node->vertices);
    get_vertice->parent = NULL;
    get_vertice->collapsible = true;
    get_vertice->normal = normal;
    get_vertice->euler = 1;
    memcpy(get_vertice->eis, ei, sizeof(int) * 12);
    get_vertice->in_cell = octree_node->child_index;
    get_vertice->face_prop2 = true;
    vec3 emp_buffer = VEC3_ZERO;
    qef_solver_solve(&get_vertice->qef, &emp_buffer, 1e-6f, 4, 1e-6f);
    get_vertice->error = qef_solver_get_error_pos(&get_vertice->qef, get_vertice->qef.x);
  }

  return true;
}

// TODO: Optimize above as much as possible

void manifold_octree_process_cell_split_threads(struct ManifoldOctreeNode* octree_node, struct Vector* indexes, float threshold) {
  if (octree_node->type == MANIFOLD_NODE_INTERNAL) {
    for (int i = 0; i < 8; i++) {
      if (octree_node->children[i] != NULL)
        manifold_octree_process_cell_split_threads(octree_node->children[i], indexes, threshold);
    }

    for (int i = 0; i < 12; i++) {
      struct ManifoldOctreeNode* face_nodes[2] = {NULL};

      int c1 = TEdgePairs[i][0];
      int c2 = TEdgePairs[i][1];

      face_nodes[0] = octree_node->children[c1];
      face_nodes[1] = octree_node->children[c2];

      manifold_octree_process_face(face_nodes, TEdgePairs[i][2], indexes, threshold);
    }

    for (int i = 0; i < 6; i++) {
      struct ManifoldOctreeNode* edge_nodes[4] = {octree_node->children[TCellProcEdgeMask[i][0]], octree_node->children[TCellProcEdgeMask[i][1]], octree_node->children[TCellProcEdgeMask[i][2]], octree_node->children[TCellProcEdgeMask[i][3]]};

      manifold_octree_process_edge(edge_nodes, TCellProcEdgeMask[i][4], indexes, threshold);
    }
  }
}

void manifold_octree_process_cell(struct ManifoldOctreeNode* octree_node, struct Vector* indexes, float threshold) {
  struct Vector index_vectors[8] = {0};
  for (int index_num = 0; index_num < 8; index_num++)
    vector_init(&index_vectors[index_num], sizeof(uint32_t));

  // Spawn thread for each octree child
  if (octree_node->type == MANIFOLD_NODE_INTERNAL) {
#pragma omp parallel for num_threads(omp_get_max_threads())
    for (int i = 0; i < 8; i++) {
      if (octree_node->children[i] != NULL)
        manifold_octree_process_cell_split_threads(octree_node->children[i], &index_vectors[i], threshold);
    }

    // Combine child indices
    for (int vector_num = 0; vector_num < 8; vector_num++) {
      for (int vert_num = 0; vert_num < vector_size(&index_vectors[vector_num]); vert_num++)
        mesh_assign_indice(indexes, *(uint32_t*)vector_get(&index_vectors[vector_num], vert_num));
    }

    for (int i = 0; i < 12; i++) {
      struct ManifoldOctreeNode* face_nodes[2] = {NULL};

      int c1 = TEdgePairs[i][0];
      int c2 = TEdgePairs[i][1];

      face_nodes[0] = octree_node->children[c1];
      face_nodes[1] = octree_node->children[c2];

      manifold_octree_process_face(face_nodes, TEdgePairs[i][2], indexes, threshold);
    }

    for (int i = 0; i < 6; i++) {
      struct ManifoldOctreeNode* edge_nodes[4] = {octree_node->children[TCellProcEdgeMask[i][0]], octree_node->children[TCellProcEdgeMask[i][1]], octree_node->children[TCellProcEdgeMask[i][2]], octree_node->children[TCellProcEdgeMask[i][3]]};

      manifold_octree_process_edge(edge_nodes, TCellProcEdgeMask[i][4], indexes, threshold);
    }
  }

  for (int index_num = 0; index_num < 8; index_num++)
    vector_delete(&index_vectors[index_num]);
}

void manifold_octree_process_face(struct ManifoldOctreeNode* nodes[2], int direction, struct Vector* indexes, float threshold) {
  if (nodes[0] == NULL || nodes[1] == NULL)
    return;

  if (nodes[0]->type != MANIFOLD_NODE_LEAF || nodes[1]->type != MANIFOLD_NODE_LEAF) {
    for (int i = 0; i < 4; i++) {
      struct ManifoldOctreeNode* face_nodes[2] = {NULL};

      for (int j = 0; j < 2; j++) {
        if (nodes[j]->type == MANIFOLD_NODE_LEAF)
          face_nodes[j] = nodes[j];
        else
          face_nodes[j] = nodes[j]->children[TFaceProcFaceMask[direction][i][j]];
      }

      manifold_octree_process_face(face_nodes, TFaceProcFaceMask[direction][i][2], indexes, threshold);
    }

    int orders[2][4] = {{0, 0, 1, 1}, {0, 1, 0, 1}};

    for (int i = 0; i < 4; i++) {
      struct ManifoldOctreeNode* edge_nodes[4] = {NULL};

      for (int j = 0; j < 4; j++) {
        if (nodes[orders[TFaceProcEdgeMask[direction][i][0]][j]]->type == MANIFOLD_NODE_LEAF)
          edge_nodes[j] = nodes[orders[TFaceProcEdgeMask[direction][i][0]][j]];
        else
          edge_nodes[j] = nodes[orders[TFaceProcEdgeMask[direction][i][0]][j]]->children[TFaceProcEdgeMask[direction][i][1 + j]];
      }

      manifold_octree_process_edge(edge_nodes, TFaceProcEdgeMask[direction][i][5], indexes, threshold);
    }
  }
}

void manifold_octree_process_edge(struct ManifoldOctreeNode* nodes[4], int direction, struct Vector* indexes, float threshold) {
  if (nodes[0] == NULL || nodes[1] == NULL || nodes[2] == NULL || nodes[3] == NULL)
    return;

  if (nodes[0]->type == MANIFOLD_NODE_LEAF && nodes[1]->type == MANIFOLD_NODE_LEAF && nodes[2]->type == MANIFOLD_NODE_LEAF && nodes[3]->type == MANIFOLD_NODE_LEAF) {
    manifold_octree_process_indexes(nodes, direction, indexes, threshold);
  } else {
    for (int i = 0; i < 2; i++) {
      struct ManifoldOctreeNode* edge_nodes[4] = {NULL};

      for (int j = 0; j < 4; j++) {
        if (nodes[j]->type == MANIFOLD_NODE_LEAF)
          edge_nodes[j] = nodes[j];
        else
          edge_nodes[j] = nodes[j]->children[TEdgeProcEdgeMask[direction][i][j]];
      }

      manifold_octree_process_edge(edge_nodes, TEdgeProcEdgeMask[direction][i][4], indexes, threshold);
    }
  }
}

void manifold_octree_process_indexes(struct ManifoldOctreeNode* nodes[4], int direction, struct Vector* indexes, float threshold) {
  int min_size = 10000000;
  int indices[4] = {-1, -1, -1, -1};
  bool flip = false;
  bool sign_changed = false;
  int v_count = 0;

  for (int i = 0; i < 4; i++) {
    int edge = TProcessEdgeMask[direction][i];
    int c1 = TEdgePairs[edge][0];
    int c2 = TEdgePairs[edge][1];

    int m1 = (nodes[i]->corners >> c1) & 1;
    int m2 = (nodes[i]->corners >> c2) & 1;

    if (nodes[i]->size < min_size) {
      min_size = nodes[i]->size;
      flip = m1 == 1;
      sign_changed = ((m1 == 0 && m2 != 0) || (m1 != 0 && m2 == 0));
    }

    int index = 0;
    bool skip = false;
    for (int k = 0; k < 16; k++) {
      int e = TransformedEdgesTable[nodes[i]->corners][k];
      if (e == -1) {
        index++;
        continue;
      }
      if (e == -2) {
        skip = true;
        break;
      }
      if (e == edge)
        break;
    }

    if (skip)
      continue;

    v_count++;
    if (index >= array_list_size(nodes[i]->vertices))
      return;
    struct Vertex* v = (struct Vertex*)array_list_get(nodes[i]->vertices, index);
    struct Vertex* highest = v;
    while (highest->parent != NULL) {
      if (highest->parent->error <= threshold && (highest->parent->euler == 1 && highest->parent->face_prop2))
        highest = v = highest->parent;
      else
        highest = highest->parent;
    }

    indices[i] = v->index;
  }

  if (sign_changed) {
    if (!flip) {
      if (indices[0] != -1 && indices[1] != -1 && indices[2] != -1 && indices[0] != indices[1] && indices[1] != indices[3]) {
        mesh_assign_indice(indexes, indices[0]);
        mesh_assign_indice(indexes, indices[1]);
        mesh_assign_indice(indexes, indices[3]);
      }

      if (indices[0] != -1 && indices[2] != -1 && indices[3] != -1 && indices[0] != indices[2] && indices[2] != indices[3]) {
        mesh_assign_indice(indexes, indices[0]);
        mesh_assign_indice(indexes, indices[3]);
        mesh_assign_indice(indexes, indices[2]);
      }
    } else {
      if (indices[0] != -1 && indices[3] != -1 && indices[1] != -1 && indices[0] != indices[1] && indices[1] != indices[3]) {
        mesh_assign_indice(indexes, indices[0]);
        mesh_assign_indice(indexes, indices[3]);
        mesh_assign_indice(indexes, indices[1]);
      }

      if (indices[0] != -1 && indices[2] != -1 && indices[3] != -1 && indices[0] != indices[2] && indices[2] != indices[3]) {
        mesh_assign_indice(indexes, indices[0]);
        mesh_assign_indice(indexes, indices[2]);
        mesh_assign_indice(indexes, indices[3]);
      }
    }
  }
}

void manifold_octree_cluster_cell_base(struct ManifoldOctreeNode* octree_node, float error) {
  if (octree_node->type != MANIFOLD_NODE_INTERNAL)
    return;

#pragma omp parallel for num_threads(omp_get_max_threads())
  for (int i = 0; i < 8; i++) {
    if (octree_node->children[i] == NULL)
      continue;

    manifold_octree_cluster_cell(octree_node->children[i], error);
  }
}

static inline void manifold_octree_cluster_cell(struct ManifoldOctreeNode* octree_node, float error) {
  if (octree_node->type != MANIFOLD_NODE_INTERNAL)
    return;

  int signs[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
  int mid_sign = -1;

  for (int i = 0; i < 8; i++) {
    if (octree_node->children[i] == NULL)
      continue;

    manifold_octree_cluster_cell(octree_node->children[i], error);
    if (octree_node->children[i]->type != MANIFOLD_NODE_INTERNAL) {
      mid_sign = (octree_node->children[i]->corners >> (7 - i)) & 1;
      signs[i] = (octree_node->children[i]->corners >> i) & 1;
    }
  }

  octree_node->corners = 0;
  for (int i = 0; i < 8; i++) {
    if (signs[i] == -1)
      octree_node->corners |= (unsigned char)(mid_sign << i);
    else
      octree_node->corners |= (unsigned char)(signs[i] << i);
  }

  int surface_index = 0;
  struct ArrayList collected_vertices = {0};
  array_list_init(&collected_vertices);
  struct ArrayList* new_vertices = calloc(1, sizeof(struct ArrayList));
  array_list_init(new_vertices);

  for (int i = 0; i < 12; i++) {
    struct ManifoldOctreeNode* face_nodes[2] = {NULL};

    int c1 = TEdgePairs[i][0];
    int c2 = TEdgePairs[i][1];

    face_nodes[0] = octree_node->children[c1];
    face_nodes[1] = octree_node->children[c2];

    manifold_octree_cluster_face(face_nodes, TEdgePairs[i][2], &surface_index, &collected_vertices);
  }

  for (int i = 0; i < 6; i++) {
    struct ManifoldOctreeNode* edge_nodes[4] = {octree_node->children[TCellProcEdgeMask[i][0]], octree_node->children[TCellProcEdgeMask[i][1]], octree_node->children[TCellProcEdgeMask[i][2]], octree_node->children[TCellProcEdgeMask[i][3]]};
    manifold_octree_cluster_edge(edge_nodes, TCellProcEdgeMask[i][4], &surface_index, &collected_vertices);
  }

  int highest_index = surface_index;

  if (highest_index == -1)
    highest_index = 0;

  for (int octree_num = 0; octree_num < 8; octree_num++) {
    struct ManifoldOctreeNode* n = octree_node->children[octree_num];
    if (n == NULL)
      continue;

    for (int vertice_num = 0; vertice_num < array_list_size(n->vertices); vertice_num++) {
      struct Vertex* v = (struct Vertex*)array_list_get(n->vertices, vertice_num);
      if (v == NULL)
        continue;
      if (v->surface_index == -1) {
        v->surface_index = highest_index++;
        array_list_add(&collected_vertices, v);
      }
    }
  }

  if (array_list_size(&collected_vertices) > 0) {
    for (int i = 0; i <= highest_index; i++) {
      struct QefSolver qef = {0};
      qef_solver_init(&qef);
      vec3 normal = VEC3_ZERO;
      int count = 0;
      int edges[12] = {0};
      int euler = 0;
      int e = 0;

      for (int vertice_num = 0; vertice_num < array_list_size(&collected_vertices); vertice_num++) {
        struct Vertex* v = (struct Vertex*)array_list_get(&collected_vertices, vertice_num);
        if (v->surface_index == i) {
          for (int k = 0; k < 3; k++) {
            int edge = TExternalEdges[v->in_cell][k];
            edges[edge] += v->eis[edge];
          }
          for (int k = 0; k < 9; k++) {
            int edge = TInternalEdges[v->in_cell][k];
            e += v->eis[edge];
          }
          euler += v->euler;
          qef_solver_add_copy(&qef, &v->qef.data);
          normal = vec3_add(normal, v->normal);
          count++;
        }
      }

      if (count == 0)
        continue;

      bool face_prop2 = true;
      for (int f = 0; f < 6 && face_prop2; f++) {
        int intersections = 0;
        for (int ei = 0; ei < 4; ei++) {
          intersections += edges[TFaces[f][ei]];
        }
        if (!(intersections == 0 || intersections == 2))
          face_prop2 = false;
      }

      struct Vertex* new_vertex = calloc(1, sizeof(struct Vertex));
      vertex_init(new_vertex);
      normal = vec3_old_skool_divs(normal, count);
      normal = vec3_old_skool_normalise(normal);
      new_vertex->normal = normal;
      new_vertex->qef = qef;
      memcpy(new_vertex->eis, edges, sizeof(int) * 12);
      new_vertex->euler = euler - e / 4;
      new_vertex->in_cell = octree_node->child_index;
      new_vertex->face_prop2 = face_prop2;
      array_list_add(new_vertices, new_vertex);

      vec3 buf_extra = VEC3_ZERO;
      qef_solver_solve(&qef, &buf_extra, 1e-6f, 4, 1e-6f);
      float err = qef_solver_get_error(&qef);
      new_vertex->collapsible = err <= error;
      new_vertex->error = err;

      for (int vertice_num = 0; vertice_num < array_list_size(&collected_vertices); vertice_num++) {
        struct Vertex* v = (struct Vertex*)array_list_get(&collected_vertices, vertice_num);
        if (v->surface_index == i)
          v->parent = new_vertex;
      }
    }
  } else {
    array_list_delete(new_vertices);
    free(new_vertices);
    array_list_delete(&collected_vertices);
    return;
  }

  for (int vertice_num = 0; vertice_num < array_list_size(&collected_vertices); vertice_num++) {
    struct Vertex* v2 = (struct Vertex*)array_list_get(&collected_vertices, vertice_num);
    v2->surface_index = -1;
  }

  octree_node->vertices = new_vertices;
  array_list_delete(&collected_vertices);
}

static inline void manifold_octree_cluster_face(struct ManifoldOctreeNode* nodes[2], int direction, int* surface_index, struct ArrayList* collected_vertices) {
  if (nodes[0] == NULL || nodes[1] == NULL)
    return;

  if (nodes[0]->type != MANIFOLD_NODE_LEAF || nodes[1]->type != MANIFOLD_NODE_LEAF) {
    for (int i = 0; i < 4; i++) {
      struct ManifoldOctreeNode* face_nodes[2] = {NULL};
      for (int j = 0; j < 2; j++) {
        if (nodes[j] == NULL)
          continue;
        if (nodes[j]->type != MANIFOLD_NODE_INTERNAL)
          face_nodes[j] = nodes[j];
        else
          face_nodes[j] = nodes[j]->children[TFaceProcFaceMask[direction][i][j]];
      }

      manifold_octree_cluster_face(face_nodes, TFaceProcFaceMask[direction][i][2], surface_index, collected_vertices);
    }
  }

  int orders[2][4] = {{0, 0, 1, 1}, {0, 1, 0, 1}};

  for (int i = 0; i < 4; i++) {
    struct ManifoldOctreeNode* edge_nodes[4] = {NULL};
    for (int j = 0; j < 4; j++) {
      if (nodes[orders[TFaceProcEdgeMask[direction][i][0]][j]] == NULL)
        continue;
      if (nodes[orders[TFaceProcEdgeMask[direction][i][0]][j]]->type != MANIFOLD_NODE_INTERNAL)
        edge_nodes[j] = nodes[orders[TFaceProcEdgeMask[direction][i][0]][j]];
      else
        edge_nodes[j] = nodes[orders[TFaceProcEdgeMask[direction][i][0]][j]]->children[TFaceProcEdgeMask[direction][i][1 + j]];
    }

    manifold_octree_cluster_edge(edge_nodes, TFaceProcEdgeMask[direction][i][5], surface_index, collected_vertices);
  }
}

static inline void manifold_octree_cluster_edge(struct ManifoldOctreeNode* nodes[4], int direction, int* surface_index, struct ArrayList* collected_vertices) {
  if ((nodes[0] == NULL || nodes[0]->type != MANIFOLD_NODE_INTERNAL) && (nodes[1] == NULL || nodes[1]->type != MANIFOLD_NODE_INTERNAL) && (nodes[2] == NULL || nodes[2]->type != MANIFOLD_NODE_INTERNAL) && (nodes[3] == NULL || nodes[3]->type != MANIFOLD_NODE_INTERNAL)) {
    manifold_octree_cluster_indexes(nodes, direction, surface_index, collected_vertices);
  } else {
    for (int i = 0; i < 2; i++) {
      struct ManifoldOctreeNode* edge_nodes[4] = {NULL};
      for (int j = 0; j < 4; j++) {
        if (nodes[j] == NULL)
          continue;
        if (nodes[j]->type == MANIFOLD_NODE_LEAF)
          edge_nodes[j] = nodes[j];
        else
          edge_nodes[j] = nodes[j]->children[TEdgeProcEdgeMask[direction][i][j]];
      }

      manifold_octree_cluster_edge(edge_nodes, TEdgeProcEdgeMask[direction][i][4], surface_index, collected_vertices);
    }
  }
}

static inline void manifold_octree_cluster_indexes(struct ManifoldOctreeNode* nodes[8], int direction, int* max_surface_index, struct ArrayList* collected_vertices) {
  if (nodes[0] == NULL && nodes[1] == NULL && nodes[2] == NULL && nodes[3] == NULL)
    return;

  struct Vertex* vertices[4] = {NULL};
  int v_count = 0;
  int node_count = 0;

  for (int i = 0; i < 4; i++) {
    if (nodes[i] == NULL)
      continue;
    node_count++;

    int edge = TProcessEdgeMask[direction][i];
    int c1 = TEdgePairs[edge][0];
    int c2 = TEdgePairs[edge][1];

    int m1 = (nodes[i]->corners >> c1) & 1;
    int m2 = (nodes[i]->corners >> c2) & 1;

    int index = 0;
    bool skip = false;
    for (int k = 0; k < 16; k++) {
      int e = TransformedEdgesTable[nodes[i]->corners][k];
      if (e == -1) {
        index++;
        continue;
      }
      if (e == -2) {
        if (!((m1 == 0 && m2 != 0) || (m1 != 0 && m2 == 0)))
          skip = true;
        break;
      }
      if (e == edge)
        break;
    }

    if (!skip && index < array_list_size(nodes[i]->vertices)) {
      vertices[i] = (struct Vertex*)array_list_get(nodes[i]->vertices, index);
      while (vertices[i]->parent != NULL)
        vertices[i] = vertices[i]->parent;
      v_count++;
    }
  }

  if (v_count == 0)
    return;

  int surface_index = -1;

  for (int i = 0; i < 4; i++) {
    struct Vertex* v = vertices[i];
    if (v == NULL)
      continue;

    if (v->surface_index != -1) {
      if (surface_index != -1 && surface_index != v->surface_index) {
        for (int vertex_num = 0; vertex_num < array_list_size(collected_vertices); vertex_num++) {
          struct Vertex* ver = (struct Vertex*)array_list_get(collected_vertices, vertex_num);
          if (ver != NULL && ver->surface_index == v->surface_index)
            ver->surface_index = surface_index;
        }
      } else if (surface_index == -1)
        surface_index = v->surface_index;
    }
  }

  if (surface_index == -1)
    surface_index = (*max_surface_index)++;

  for (int i = 0; i < 4; i++) {
    struct Vertex* v = vertices[i];
    if (v == NULL)
      continue;
    if (v->surface_index == -1)
      array_list_add(collected_vertices, v);
    v->surface_index = surface_index;
  }
}
