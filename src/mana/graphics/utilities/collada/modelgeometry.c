#include "mana/graphics/utilities/collada/modelgeometry.h"

struct Mesh* geometry_loader_extract_model_data(struct XmlNode* geometry_node, struct Vector* vertex_weights) {
  struct XmlNode* mesh_data = xml_node_get_child(xml_node_get_child(geometry_node, "geometry"), "mesh");
  struct ModelData* model_data = malloc(sizeof(struct ModelData));
  model_data_init(model_data);

  geometry_loader_read_raw_data(model_data, mesh_data, vertex_weights);
  geometry_loader_assemble_vertices(model_data, mesh_data);
  geometry_loader_remove_unused_vertices(model_data);

  struct Mesh* model_mesh = malloc(sizeof(struct Mesh));
  mesh_model_init(model_mesh);
  geometry_loader_convert_data_to_arrays(model_data, model_mesh);
  model_mesh->indices = model_data->indices;

  return model_mesh;
}

void geometry_loader_read_raw_data(struct ModelData* model_data, struct XmlNode* mesh_data, struct Vector* vertex_weights) {
  geometry_loader_read_positions(model_data, mesh_data, vertex_weights);
  geometry_loader_read_normals(model_data, mesh_data);
  geometry_loader_read_texture_coordinates(model_data, mesh_data);
}

void geometry_loader_read_positions(struct ModelData* model_data, struct XmlNode* mesh_data, struct Vector* vertex_weights) {
  char* positions_id = xml_node_get_attribute(xml_node_get_child(xml_node_get_child(mesh_data, "vertices"), "input"), "source") + 1;
  struct XmlNode* positions_data = xml_node_get_child(xml_node_get_child_with_attribute(mesh_data, "source", "id", positions_id), "float_array");
  int count = atoi(xml_node_get_attribute(positions_data, "count"));

  char* raw_data = strdup(xml_node_get_data(positions_data));
  char* raw_part = strtok(raw_data, " ");
  while (raw_part != NULL) {
    float x_pos = atof(raw_part);
    raw_part = strtok(NULL, " ");
    float y_pos = atof(raw_part);
    raw_part = strtok(NULL, " ");
    float z_pos = atof(raw_part);
    raw_part = strtok(NULL, " ");

    vec4 position = {x_pos, y_pos, z_pos, 1.0f};
    mat4 correction = GLM_MAT4_IDENTITY_INIT;
    glm_rotate(correction, glm_rad(-90.0f), (vec3){1.0f, 0.0f, 0.0f});
    glm_mat4_mulv(correction, position, position);
    vec3 position_corrected = {position[0], position[1], position[2]};
    struct RawVertexModel* raw_vertex = malloc(sizeof(struct RawVertexModel));
    raw_vertex_model_init(raw_vertex, vector_size(model_data->vertices), position_corrected, (struct VertexSkinData*)vector_get(vertex_weights, vector_size(model_data->vertices)));
    vector_push_back(model_data->vertices, raw_vertex);
  }
  free(raw_data);
}

void geometry_loader_read_normals(struct ModelData* model_data, struct XmlNode* mesh_data) {
  char* normals_id = xml_node_get_attribute(xml_node_get_child_with_attribute(xml_node_get_child(mesh_data, "polylist"), "input", "semantic", "NORMAL"), "source") + 1;
  struct XmlNode* normals_data = xml_node_get_child(xml_node_get_child_with_attribute(mesh_data, "source", "id", normals_id), "float_array");
  int count = atoi(xml_node_get_attribute(normals_data, "count"));

  char* raw_data = strdup(xml_node_get_data(normals_data));
  char* raw_part = strtok(raw_data, " ");
  while (raw_part != NULL) {
    float x_norm = atof(raw_part);
    raw_part = strtok(NULL, " ");
    float y_norm = atof(raw_part);
    raw_part = strtok(NULL, " ");
    float z_norm = atof(raw_part);
    raw_part = strtok(NULL, " ");

    vec4 normal = {x_norm, y_norm, z_norm, 0.0f};
    mat4 correction = GLM_MAT4_IDENTITY_INIT;
    glm_rotate(correction, glm_rad(-90.0f), (vec3){1.0f, 0.0f, 0.0f});
    glm_mat4_mulv(correction, normal, normal);
    vec3 normal_corrected = {normal[0], normal[1], normal[2]};
    vector_push_back(model_data->normals, &normal_corrected);
  }
  free(raw_data);
}

void geometry_loader_read_texture_coordinates(struct ModelData* model_data, struct XmlNode* mesh_data) {
  char* tex_coords_id = xml_node_get_attribute(xml_node_get_child_with_attribute(xml_node_get_child(mesh_data, "polylist"), "input", "semantic", "TEXCOORD"), "source") + 1;
  struct XmlNode* tex_coords_data = xml_node_get_child(xml_node_get_child_with_attribute(mesh_data, "source", "id", tex_coords_id), "float_array");
  int count = atoi(xml_node_get_attribute(tex_coords_data, "count"));

  char* raw_data = strdup(xml_node_get_data(tex_coords_data));
  char* raw_part = strtok(raw_data, " ");
  while (raw_part != NULL) {
    float s = atof(raw_part);
    raw_part = strtok(NULL, " ");
    float t = atof(raw_part);
    raw_part = strtok(NULL, " ");
    vec2 tex_coord = {s, t};
    vector_push_back(model_data->tex_coords, &tex_coord);
  }
  free(raw_data);
}

void geometry_loader_assemble_vertices(struct ModelData* model_data, struct XmlNode* mesh_data) {
  struct XmlNode* poly = xml_node_get_child(mesh_data, "polylist");
  struct XmlNode* index_data = xml_node_get_child(poly, "p");

  char* raw_data = strdup(xml_node_get_data(index_data));
  char* raw_part = strtok(raw_data, " ");
  while (raw_part != NULL) {
    int position_index = atoi(raw_part);
    raw_part = strtok(NULL, " ");
    int normal_index = atoi(raw_part);
    raw_part = strtok(NULL, " ");
    int tex_coord_index = atoi(raw_part);
    raw_part = strtok(NULL, " ");
    int color_index = atoi(raw_part);
    raw_part = strtok(NULL, " ");

    geometry_loader_process_vertex(model_data, position_index, normal_index, tex_coord_index);
  }
  free(raw_data);
}

struct RawVertexModel* geometry_loader_process_vertex(struct ModelData* model_data, int position_index, int normal_index, int tex_coord_index) {
  struct RawVertexModel* current_vertex = (struct RawVertexModel*)vector_get(model_data->vertices, position_index);
  if (raw_vertex_model_is_set(current_vertex) == false) {
    current_vertex->texture_index = tex_coord_index;
    current_vertex->normal_index = normal_index;
    vector_push_back(model_data->indices, &position_index);
    return current_vertex;
  } else
    return geometry_loader_deal_with_already_processed_vertex(model_data, current_vertex, tex_coord_index, normal_index);
}

float geometry_loader_convert_data_to_arrays(struct ModelData* model_data, struct Mesh* model_mesh) {
  float furthest_point = 0.0f;
  for (int vertex_num = 0; vertex_num < vector_size(model_data->vertices); vertex_num++) {
    struct RawVertexModel* current_vertex = (struct RawVertexModel*)vector_get(model_data->vertices, vertex_num);
    if (current_vertex->length > furthest_point)
      furthest_point = current_vertex->length;

    struct VertexSkinData* model_weights = current_vertex->weights_data;
    vec3 model_position = GLM_VEC3_ZERO_INIT;
    glm_vec3_copy(current_vertex->position, model_position);
    vec3 model_normal = GLM_VEC3_ZERO_INIT;
    glm_vec3_copy(vector_get(model_data->normals, current_vertex->normal_index), model_normal);
    vec2 model_tex_coord = {0.0f, 0.0f};
    memcpy(model_tex_coord, (vec2*)vector_get(model_data->tex_coords, current_vertex->texture_index), sizeof(vec2));
    model_tex_coord[1] = 1.0f - model_tex_coord[1];
    ivec3 joint_ids = {0};
    memcpy(joint_ids, (ivec3*)vector_get(model_weights->joint_ids, 0), sizeof(ivec3));
    vec3 joint_weights = GLM_VEC3_ZERO_INIT;
    memcpy(joint_weights, (vec3*)vector_get(model_weights->weights, 0), sizeof(vec3));

    mesh_model_assign_vertex(model_mesh->vertices, model_position[0], model_position[1], model_position[2], model_normal[0], model_normal[1], model_normal[2], model_tex_coord[0], model_tex_coord[1], joint_ids[0], joint_ids[1], joint_ids[2], joint_weights[0], joint_weights[1], joint_weights[2]);
  }

  return furthest_point;
}

struct RawVertexModel* geometry_loader_deal_with_already_processed_vertex(struct ModelData* model_data, struct RawVertexModel* previous_vertex, int new_texture_index, int new_normal_index) {
  if (raw_vertex_model_has_same_texture_and_normal(previous_vertex, new_texture_index, new_normal_index)) {
    vector_push_back(model_data->indices, &previous_vertex->index);
    return previous_vertex;
  } else {
    struct RawVertexModel* another_vertex = previous_vertex->duplicate_vertex;
    if (another_vertex != NULL)
      return geometry_loader_deal_with_already_processed_vertex(model_data, another_vertex, new_texture_index, new_normal_index);
    else {
      struct RawVertexModel* duplicate_vertex = malloc(sizeof(struct RawVertexModel));
      raw_vertex_model_init(duplicate_vertex, vector_size(model_data->vertices), previous_vertex->position, previous_vertex->weights_data);
      duplicate_vertex->texture_index = new_texture_index;
      duplicate_vertex->normal_index = new_normal_index;
      previous_vertex->duplicate_vertex = duplicate_vertex;
      vector_push_back(model_data->vertices, duplicate_vertex);
      vector_push_back(model_data->indices, &duplicate_vertex->index);
      return duplicate_vertex;
    }
  }
}

void geometry_loader_remove_unused_vertices(struct ModelData* model_data) {
  for (int vertex_num = 0; vertex_num < vector_size(model_data->vertices); vertex_num++) {
    // Average tangents might be needed
    struct RawVertexModel* vertex = (struct RawVertexModel*)vector_get(model_data->vertices, vertex_num);
    if (raw_vertex_model_is_set(vertex) == false) {
      vertex->texture_index = 0;
      vertex->normal_index = 0;
    }
  }
}
