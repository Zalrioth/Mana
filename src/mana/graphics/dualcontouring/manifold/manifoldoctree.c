#include "mana/graphics/dualcontouring/manifold/manifoldoctree.h"

void manifold_octree_construct_base(struct ManifoldOctreeNode* octree_node, int size, float error, struct Vector* vertices) {
  octree_node->index = 0;
  octree_node->position = VEC3_ZERO;
  octree_node->size = size;
  octree_node->type = MANIFOLD_NODE_INTERNAL;
  //this.children = new ManifoldOctreeNode[8];
  //this.vertices = new Vertex[0];
  octree_node->child_index = 0;
  int n_index = 1;
  manifold_octree_construct_nodes(octree_node, vertices, &n_index);
}

void manifold_octree_generate_vertex_buffer(struct ManifoldOctreeNode* octree_node, struct Vector* vertices) {
  if (octree_node->type != MANIFOLD_NODE_LEAF) {
    for (int i = 0; i < 8; i++) {
      if (octree_node->children[i] != NULL)
        manifold_octree_generate_vertex_buffer(octree_node->children[i], vertices);
    }
  }

  if (vertices == NULL || vector_size(vertices) == 0)
    return;

  for (int i = 0; i < vector_size(vertices); i++) {
    struct Vertex* ver = (struct Vertex*)vector_get(vertices, i);
    if (ver == NULL)
      continue;
    ver->index = vector_size(vertices);
    struct Vertex* find_ver = (struct Vertex*)vector_get(octree_node->vertices, i);
    vec3 nc = vec3_normalise(vec3_add(vec3_scale(ver->normal, 0.5f), vec3_scale(VEC3_ONE, 0.5f)));
    vec3 solved_x = VEC3_ZERO;
    qef_solver_solve(&find_ver->qef, &solved_x, 1e-6f, 4, 1e-6f);
    struct VertexManifoldDualContouring new_ver = (struct VertexManifoldDualContouring){.position = solved_x, .color = nc, .normal1 = find_ver->normal, .normal2 = find_ver->normal};
    vector_push_back(octree_node->vertices, &new_ver);
  }
}

bool manifold_octree_construct_nodes(struct ManifoldOctreeNode* octree_node, struct Vector* vertices, int* n_index) {
  if (octree_node->size == 1)
    return manifold_octree_construct_leaf(octree_node, vertices, n_index);

  octree_node->type = MANIFOLD_NODE_INTERNAL;
  int child_size = octree_node->size / 2;
  bool has_children = false;
  bool return_values[8];

  for (int i = 0; i < 8; i++) {
    octree_node->index = (*n_index)++;
    vec3 child_pos = TCornerDeltas[i];
    struct ManifoldOctreeNode* new_node = calloc(1, sizeof(struct ManifoldOctreeNode));
    octree_node_init(new_node, vec3_add(octree_node->position, vec3_scale(child_pos, (float)child_size)), child_size, MANIFOLD_NODE_INTERNAL);
    octree_node->children[i] = new_node;
    octree_node->children[i]->child_index = i;

    int index = i;
    if (octree_node->size > 2) {
      int temp = 0;
      return_values[index] = manifold_octree_construct_nodes(octree_node->children[index], vertices, &temp);
      if (!return_values[index])
        octree_node->children[index] = NULL;
    } else {
      if (manifold_octree_construct_nodes(octree_node->children[i], vertices, n_index))
        has_children = true;
      else
        octree_node->children[i] = NULL;
    }
  }

  if (octree_node->size > 2) {
    for (int i = 0; i < 8; i++) {
      if (return_values[i])
        has_children = true;
    }
  }

  return has_children;
}

bool manifold_octree_construct_leaf(struct ManifoldOctreeNode* octree_node, struct Vector* vertices, int* index) {
  if (octree_node->size != 1)
    return false;

  octree_node->index = (*index)++;
  octree_node->type = MANIFOLD_NODE_LEAF;
  int corners = 0;
  float samples[8];
  for (int i = 0; i < 8; i++) {
    if ((samples[i] = Sphere(vec3_add(octree_node->position, TCornerDeltas[i]))) < 0)
      corners |= 1 << i;
  }
  octree_node->corners = (char)corners;

  if (corners == 0 || corners == 255)
    return false;

  int v_edges[TransformedVerticesNumberTable[octree_node->corners]][16];
  octree_node->vertices = calloc(1, sizeof(struct Vector));
  vector_init(octree_node->vertices, sizeof(struct Vertex));
  for (int add_num = 0; add_num < TransformedVerticesNumberTable[corners]; add_num++) {
    struct Vertex temp = {0};
    vector_push_back(octree_node->vertices, &temp);
  }

  int v_index = 0;
  int e_index = 0;
  memset(v_edges, -1, sizeof(int) * 8);
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

  for (int i = 0; i < v_index; i++) {
    int k = 0;
    struct Vertex new_vertex;
    vertex_init(&new_vertex);
    vector_push_back(octree_node->vertices, &new_vertex);
    struct Vertex* get_vertice = (struct Vertex*)vector_get(octree_node->vertices, i);
    //get_vertice->qef = new QEFProper.QEFSolver();
    qef_solver_init(&get_vertice->qef);
    vec3 normal = VEC3_ZERO;
    int ei[12];
    while (v_edges[i][k] != -1) {
      ei[v_edges[i][k]] = 1;
      vec3 a = vec3_add(octree_node->position, vec3_scale(TCornerDeltas[TEdgePairs[v_edges[i][k]][0]], octree_node->size));
      vec3 b = vec3_add(octree_node->position, vec3_scale(TCornerDeltas[TEdgePairs[v_edges[i][k]][1]], octree_node->size));
      vec3 intersection = GetIntersection(a, b, samples[TEdgePairs[v_edges[i][k]][0]], samples[TEdgePairs[v_edges[i][k]][1]]);
      vec3 n = GetNormal(intersection);
      normal = vec3_add(normal, n);
      qef_solver_add(&get_vertice->qef, intersection.x, intersection.y, intersection.z, normal.x, normal.y, normal.z);
      k++;
    }

    normal = vec3_normalise(vec3_divs(normal, (float)k));
    get_vertice->index = vector_size(octree_node->vertices);
    get_vertice->parent = NULL;
    get_vertice->collapsible = true;
    get_vertice->normal = normal;
    get_vertice->euler = 1;
    memcpy(get_vertice->eis, ei, sizeof(int) * 12);
    get_vertice->in_cell = octree_node->child_index;
    get_vertice->face_prop2 = true;
    vec3 emp_buffer;
    qef_solver_solve(&get_vertice->qef, &emp_buffer, 1e-6f, 4, 1e-6f);
    get_vertice->error = qef_solver_get_error_pos(&get_vertice->qef, get_vertice->qef.x);
  }

  return true;
}

void manifold_octree_process_cell(struct ManifoldOctreeNode* octree_node, struct Vector* indexes, struct Vector* tri_count, float threshold) {
  if (octree_node->type == MANIFOLD_NODE_INTERNAL) {
    for (int i = 0; i < 8; i++) {
      if (octree_node->children[i] != NULL)
        manifold_octree_process_cell(octree_node, indexes, tri_count, threshold);
    }

    for (int i = 0; i < 12; i++) {
      struct ManifoldOctreeNode* face_nodes[2] = {NULL};

      int c1 = TEdgePairs[i][0];
      int c2 = TEdgePairs[i][1];

      face_nodes[0] = octree_node->children[c1];
      face_nodes[1] = octree_node->children[c2];

      manifold_octree_process_face(face_nodes, TEdgePairs[i][2], indexes, tri_count, threshold);
    }

    for (int i = 0; i < 6; i++) {
      struct ManifoldOctreeNode* edge_nodes[4] = {octree_node->children[TCellProcEdgeMask[i][0]], octree_node->children[TCellProcEdgeMask[i][1]], octree_node->children[TCellProcEdgeMask[i][2]], octree_node->children[TCellProcEdgeMask[i][3]]};

      manifold_octree_process_edge(edge_nodes, TCellProcEdgeMask[i][4], indexes, tri_count, threshold);
    }
  }
}

void manifold_octree_process_face(struct ManifoldOctreeNode* nodes[2], int direction, struct Vector* indexes, struct Vector* tri_count, float threshold) {
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

      manifold_octree_process_face(face_nodes, TFaceProcFaceMask[direction][i][2], indexes, tri_count, threshold);
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

      manifold_octree_process_edge(edge_nodes, TFaceProcEdgeMask[direction][i][5], indexes, tri_count, threshold);
    }
  }
}

void manifold_octree_process_edge(struct ManifoldOctreeNode* nodes[4], int direction, struct Vector* indexes, struct Vector* tri_count, float threshold) {
  if (nodes[0] == NULL || nodes[1] == NULL || nodes[2] == NULL || nodes[3] == NULL)
    return;

  if (nodes[0]->type == MANIFOLD_NODE_LEAF && nodes[1]->type == MANIFOLD_NODE_LEAF && nodes[2]->type == MANIFOLD_NODE_LEAF && nodes[3]->type == MANIFOLD_NODE_LEAF) {
    manifold_octree_process_edge(nodes, direction, indexes, tri_count, threshold);
  } else {
    for (int i = 0; i < 2; i++) {
      struct ManifoldOctreeNode* edge_nodes[4] = {NULL};

      for (int j = 0; j < 4; j++) {
        if (nodes[j]->type == MANIFOLD_NODE_LEAF)
          edge_nodes[j] = nodes[j];
        else
          edge_nodes[j] = nodes[j]->children[TEdgeProcEdgeMask[direction][i][j]];
      }

      manifold_octree_process_edge(edge_nodes, TEdgeProcEdgeMask[direction][i][4], indexes, tri_count, threshold);
    }
  }
}

void manifold_octree_process_indexes(struct ManifoldOctreeNode* nodes[4], int direction, struct Vector* indexes, struct Vector* tri_count, float threshold) {
  int min_size = 10000000;
  int min_index = 0;
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
      min_index = i;
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
    if (index >= vector_size(nodes[i]->vertices))
      return;
    struct Vertex* v = (struct Vertex*)vector_get(nodes[i]->vertices, index);
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
    int count = 0;
    if (!flip) {
      if (indices[0] != -1 && indices[1] != -1 && indices[2] != -1 && indices[0] != indices[1] && indices[1] != indices[3]) {
        vector_push_back(indexes, &indices[0]);
        vector_push_back(indexes, &indices[1]);
        vector_push_back(indexes, &indices[3]);
        count++;
      }

      if (indices[0] != -1 && indices[2] != -1 && indices[3] != -1 && indices[0] != indices[2] && indices[2] != indices[3]) {
        vector_push_back(indexes, &indices[0]);
        vector_push_back(indexes, &indices[3]);
        vector_push_back(indexes, &indices[2]);
        count++;
      }
    } else {
      if (indices[0] != -1 && indices[3] != -1 && indices[1] != -1 && indices[0] != indices[1] && indices[1] != indices[3]) {
        int num1 = 0x10000000 | indices[0], num2 = 0x10000000 | indices[3], num3 = 0x10000000 | indices[1];
        vector_push_back(indexes, &num1);
        vector_push_back(indexes, &num2);
        vector_push_back(indexes, &num3);
        count++;
      }

      if (indices[0] != -1 && indices[2] != -1 && indices[3] != -1 && indices[0] != indices[2] && indices[2] != indices[3]) {
        int num1 = 0x10000000 | indices[0], num2 = 0x10000000 | indices[2], num3 = 0x10000000 | indices[3];
        vector_push_back(indexes, &num1);
        vector_push_back(indexes, &num2);
        vector_push_back(indexes, &num3);
        count++;
      }
    }

    if (count > 0)
      vector_push_back(tri_count, &count);
  }
}

void manifold_octree_cluster_cell_base(struct ManifoldOctreeNode* octree_node, float error) {
  if (octree_node->type != MANIFOLD_NODE_INTERNAL)
    return;

  for (int i = 0; i < 8; i++) {
    if (octree_node->children[i] == NULL)
      continue;

    manifold_octree_cluster_cell(octree_node->children[i], error);
  }
}

void manifold_octree_cluster_cell(struct ManifoldOctreeNode* octree_node, float error) {
  static int counter = 0;
  counter++;
  printf("Numb: %d\n", counter);

  if (octree_node->type != MANIFOLD_NODE_INTERNAL)
    return;

  int signs[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
  int mid_sign = -1;

  bool is_collapsible = true;
  for (int i = 0; i < 8; i++) {
    if (octree_node->children[i] == NULL)
      continue;

    manifold_octree_cluster_cell(octree_node->children[i], error);
    if (octree_node->children[i]->type == MANIFOLD_NODE_INTERNAL)
      is_collapsible = false;
    else {
      mid_sign = (octree_node->children[i]->corners >> (7 - i)) & 1;
      signs[i] = (octree_node->children[i]->corners >> i) & 1;
    }
  }

  octree_node->corners = 0;
  for (int i = 0; i < 8; i++) {
    if (signs[i] == -1)
      octree_node->corners |= (char)(mid_sign << i);
    else
      octree_node->corners |= (char)(signs[i] << i);
  }

  int surface_index = 0;
  struct Vector collected_vertices = {0};
  vector_init(&collected_vertices, sizeof(struct Vertex));
  struct Vector* new_vertices = calloc(1, sizeof(struct Vector));
  vector_init(new_vertices, sizeof(struct Vertex));

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

  //foreach (ManifoldOctreeNode n in children) {
  //for (int octree_num = 0; octree_num < vector_size(octree_node->children); octree_num++) {
  //  struct ManifoldOctreeNode* n = (struct ManifoldOctreeNode*)vector_get(octree_node->children, octree_num);
  for (int octree_num = 0; octree_num < 8; octree_num++) {
    struct ManifoldOctreeNode* n = octree_node->children[octree_num];
    if (n == NULL)
      continue;
    //foreach (Vertex v in n.vertices) {
    for (int vertice_num = 0; vertice_num < vector_size(n->vertices); vertice_num++) {
      struct Vertex* v = (struct Vertex*)vector_get(n->vertices, vertice_num);
      if (v == NULL)
        continue;
      if (v->surface_index == -1) {
        v->surface_index = highest_index++;
        vector_push_back(&collected_vertices, v);
        //collected_vertices.Add(v);
      }
    }
  }

  int clustered_count = 0;
  if (vector_size(&collected_vertices) > 0) {
    for (int i = 0; i <= highest_index; i++) {
      struct QefSolver qef = {0};
      qef_solver_init(&qef);
      vec3 normal = VEC3_ZERO;
      int count = 0;
      int edges[12];
      int euler = 0;
      int e = 0;
      //foreach (Vertex v in collected_vertices) {
      for (int vertice_num = 0; vertice_num < vector_size(&collected_vertices); vertice_num++) {
        struct Vertex* v = (struct Vertex*)vector_get(&collected_vertices, vertice_num);
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

      if (count == 0) {
        continue;
      }

      bool face_prop2 = true;
      for (int f = 0; f < 6 && face_prop2; f++) {
        int intersections = 0;
        for (int ei = 0; ei < 4; ei++) {
          intersections += edges[TFaces[f][ei]];
        }
        if (!(intersections == 0 || intersections == 2))
          face_prop2 = false;
      }

      struct Vertex* new_vertex = malloc(sizeof(struct Vertex));
      vertex_init(new_vertex);
      normal = vec3_normalise(vec3_divs(normal, (float)count));
      new_vertex->normal = normal;
      new_vertex->qef = qef;
      memcpy(new_vertex->eis, edges, sizeof(int) * 12);
      new_vertex->euler = euler - e / 4;
      new_vertex->in_cell = octree_node->child_index;
      new_vertex->face_prop2 = face_prop2;
      vector_push_back(new_vertices, new_vertex);

      vec3 buf_extra;
      qef_solver_solve(&qef, &buf_extra, 1e-6f, 4, 1e-6f);
      float err = qef_solver_get_error(&qef);
      new_vertex->collapsible = err <= error;
      new_vertex->error = err;
      clustered_count++;

      for (int vertice_num = 0; vertice_num < vector_size(&collected_vertices); vertice_num++) {
        struct Vertex* v = (struct Vertex*)vector_get(&collected_vertices, vertice_num);
        if (v->surface_index == i) {
          if (v != new_vertex)
            v->parent = new_vertex;
          else
            v->parent = NULL;
        }
      }
    }
  } else
    return;

  {
    //foreach (Vertex v2 in collected_vertices) {
    for (int vertice_num = 0; vertice_num < vector_size(&collected_vertices); vertice_num++) {
      struct Vertex* v2 = (struct Vertex*)vector_get(&collected_vertices, vertice_num);
      v2->surface_index = -1;
    }
  }
  octree_node->vertices = new_vertices;
  vector_delete(&collected_vertices);
}

void manifold_octree_gather_vertices(struct ManifoldOctreeNode* n, struct Vector* dest, int* surface_index) {
  if (n == NULL)
    return;
  if (n->size > 1) {
    for (int i = 0; i < 8; i++)
      manifold_octree_gather_vertices(n->children[i], dest, surface_index);
  } else {
    for (int vertex_num = 0; vertex_num < vector_size(n->vertices); vertex_num++) {
      struct Vertex* ver = (struct Vertex*)vector_get(n->vertices, vertex_num);
      if (ver->surface_index == -1) {
        ver->surface_index = (*surface_index)++;
        vector_push_back(dest, ver);
      }
    }
  }
}

void manifold_octree_cluster_face(struct ManifoldOctreeNode* nodes[2], int direction, int* surface_index, struct Vector* collected_vertices) {
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
    //for (int octree_num = 0; octree_num < 4; octree_num++)
    //  octree_node_init(&edge_nodes[octree_num]);

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

void manifold_octree_cluster_edge(struct ManifoldOctreeNode* nodes[4], int direction, int* surface_index, struct Vector* collected_vertices) {
  if ((nodes[0] == NULL || nodes[0]->type != MANIFOLD_NODE_INTERNAL) && (nodes[1] == NULL || nodes[1]->type != MANIFOLD_NODE_INTERNAL) && (nodes[2] == NULL || nodes[2]->type != MANIFOLD_NODE_INTERNAL) && (nodes[3] == NULL || nodes[3]->type != MANIFOLD_NODE_INTERNAL)) {
    manifold_octree_cluster_indexes(nodes, direction, surface_index, collected_vertices);
  } else {
    for (int i = 0; i < 2; i++) {
      struct ManifoldOctreeNode* edge_nodes[4] = {NULL};
      //for (int octree_num = 0; octree_num < 4; octree_num++)
      //  octree_node_init(&edge_nodes[octree_num]);

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

void manifold_octree_cluster_indexes(struct ManifoldOctreeNode* nodes[8], int direction, int* max_surface_index, struct Vector* collected_vertices) {
  if (nodes[0] == NULL && nodes[1] == NULL && nodes[2] == NULL && nodes[3] == NULL)
    return;

  struct Vertex vertices[4];
  for (int vertex_num = 0; vertex_num < 4; vertex_num++)
    vertex_init(&vertices[vertex_num]);

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

    if (!skip && index < vector_size(nodes[i]->vertices)) {
      vertices[i] = *(struct Vertex*)vector_get(nodes[i]->vertices, index);
      while (vertices[i].parent != NULL)
        vertices[i] = *vertices[i].parent;
      v_count++;
    }
  }

  if (v_count == 0)
    return;

  int surface_index = -1;

  for (int i = 0; i < 4; i++) {
    struct Vertex v = vertices[i];
    //if (v == NULL)
    //  continue;

    if (v.surface_index != -1) {
      if (surface_index != -1 && surface_index != v.surface_index) {
        manifold_octree_assign_surface(collected_vertices, v.surface_index, surface_index);
      } else if (surface_index == -1)
        surface_index = v.surface_index;
    }
  }

  if (surface_index == -1)
    surface_index = (*max_surface_index)++;

  for (int i = 0; i < 4; i++) {
    struct Vertex v = vertices[i];
    //if (v == NULL)
    //  continue;
    v.surface_index = surface_index;
    if (v.surface_index == -1)
      vector_push_back(collected_vertices, &v);
  }
}

static void manifold_octree_assign_surface(struct Vector* vertices, int from, int to) {
  for (int vertex_num = 0; vertex_num < vector_size(vertices); vertex_num++) {
    struct Vertex* ver = (struct Vertex*)vector_get(vertices, vertex_num);
    if (ver != NULL && ver->surface_index == from)
      ver->surface_index = to;
  }
}
