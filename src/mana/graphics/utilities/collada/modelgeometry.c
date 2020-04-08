#include "mana/graphics/utilities/collada/modelgeometry.h"

struct Mesh* geometry_loader_extract_model_data(struct XmlNode* geometry_node, struct Vector* vertex_weights) {
  struct XmlNode* mesh_data = xml_node_get_child(xml_node_get_child(geometry_node, "geometry"), "mesh");
  geomtry_leader_read_raw_data(mesh_data);

  return NULL;
}

void geomtry_leader_read_raw_data(struct XmlNode* mesh_data) {
  struct Vector vertices;
  vector_init(&vertices, sizeof(vec3));
  geometry_loader_read_positions(mesh_data, &vertices);

  struct Vector normals;
  vector_init(&normals, sizeof(vec3));
  geometry_loader_read_normals(mesh_data, &normals);

  struct Vector tex_coords;
  vector_init(&tex_coords, sizeof(vec2));
  geometry_loader_read_texture_coordinates(mesh_data, &tex_coords);
}

void geometry_loader_read_positions(struct XmlNode* mesh_data, struct Vector* vertices) {
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

    mat4 correction;
    glm_mat4_identity(correction);
    glm_rotate(correction, glm_rad(-90.0f), (vec3){1.0f, 0.0f, 0.0f});
    glm_mat4_mulv(correction, position, position);
    vector_push_back(vertices, &position);
  }
  free(raw_data);
}

void geometry_loader_read_normals(struct XmlNode* mesh_data, struct Vector* normals) {
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

    mat4 correction;
    glm_mat4_identity(correction);
    glm_rotate(correction, glm_rad(-90.0f), (vec3){1.0f, 0.0f, 0.0f});
    glm_mat4_mulv(correction, normal, normal);
    vector_push_back(normals, &normal);
  }
  free(raw_data);
}

void geometry_loader_read_texture_coordinates(struct XmlNode* mesh_data, struct Vector* tex_coords) {
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
    vector_push_back(tex_coords, &tex_coord);
  }
  free(raw_data);
}
