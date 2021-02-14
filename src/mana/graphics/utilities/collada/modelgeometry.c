#include "mana/graphics/utilities/collada/modelgeometry.h"

struct Mesh* geometry_loader_extract_model_data(struct XmlNode* geometry_node, struct Vector* vertex_weights, bool animated) {
  struct XmlNode* mesh_data = xml_node_get_child(xml_node_get_child(geometry_node, "geometry"), "mesh");
  struct ModelData* model_data = malloc(sizeof(struct ModelData));
  model_data_init(model_data);

  geometry_loader_read_raw_data(model_data, mesh_data, vertex_weights);
  geometry_loader_assemble_vertices(model_data, mesh_data);
  geometry_loader_remove_unused_vertices(model_data);

  struct Mesh* model_mesh = malloc(sizeof(struct Mesh));
  animated ? mesh_model_init(model_mesh) : mesh_model_static_init(model_mesh);

  geometry_loader_convert_data_to_arrays(model_data, model_mesh, animated);

  // TODO: Create copy function and memiry size od items for vector
  // Todo: Also this code is whack
  free(model_mesh->indices->items);
  *model_mesh->indices = *model_data->indices;
  model_mesh->indices->items = malloc(model_mesh->indices->memory_size * model_mesh->indices->capacity);
  memcpy(model_mesh->indices->items, model_data->indices->items, model_mesh->indices->memory_size * model_mesh->indices->capacity);
  //model_mesh->indices = model_data->indices;

  model_data_delete(model_data);
  free(model_data);

  return model_mesh;
}

void geometry_loader_read_raw_data(struct ModelData* model_data, struct XmlNode* mesh_data, struct Vector* vertex_weights) {
  geometry_loader_read_positions(model_data, mesh_data, vertex_weights);
  geometry_loader_read_normals(model_data, mesh_data);
  geometry_loader_read_texture_coordinates(model_data, mesh_data);
  geometry_loader_read_colors(model_data, mesh_data);
}

void geometry_loader_read_positions(struct ModelData* model_data, struct XmlNode* mesh_data, struct Vector* vertex_weights) {
  char* positions_id = xml_node_get_attribute(xml_node_get_child(xml_node_get_child(mesh_data, "vertices"), "input"), "source") + 1;
  struct XmlNode* positions_data = xml_node_get_child(xml_node_get_child_with_attribute(mesh_data, "source", "id", positions_id), "float_array");
  int count = atoi(xml_node_get_attribute(positions_data, "count"));
  int stride = atoi(xml_node_get_attribute(xml_node_get_child(xml_node_get_child(xml_node_get_child_with_attribute(mesh_data, "source", "id", positions_id), "technique_common"), "accessor"), "stride"));

  char* raw_data = strdup(xml_node_get_data(positions_data));
  char* raw_part = strtok(raw_data, " ");
  for (int position_num = 0; position_num < count && raw_part != NULL; position_num++) {
    vec4 position = {0};
    for (int dim_num = 0; dim_num < stride; dim_num++) {
      position.data[dim_num] = atof(raw_part);
      raw_part = strtok(NULL, " ");
    }

    mat4 correction = mat4_rotate(MAT4_IDENTITY, degree_to_radian(-90.0f), (vec3){.data[0] = 1.0f, .data[1] = 0.0f, .data[2] = 0.0f});
    position = mat4_mul_vec4(correction, position);
    vec3 position_corrected = (vec3){.data[0] = position.data[0], .data[1] = position.data[1], .data[2] = position.data[2]};
    struct RawVertexModel raw_vertex = {0};
    if (vertex_weights != NULL)
      raw_vertex_model_init(&raw_vertex, vector_size(model_data->vertices), position_corrected, (struct VertexSkinData*)vector_get(vertex_weights, vector_size(model_data->vertices)));
    else
      raw_vertex_model_init(&raw_vertex, vector_size(model_data->vertices), position_corrected, NULL);
    vector_push_back(model_data->vertices, &raw_vertex);
  }

  free(raw_data);
}

void geometry_loader_read_normals(struct ModelData* model_data, struct XmlNode* mesh_data) {
  struct XmlNode* material_node = xml_node_get_child(mesh_data, "polylist");
  if (material_node == NULL)
    material_node = xml_node_get_child(mesh_data, "triangles");
  // Note: Not sure why but Maya exports sphere mesh incorrectly
  //if (xml_node_get_child_with_attribute(material_node, "input", "semantic", "NORMAL") == NULL)
  //  material_node = xml_node_get_child(mesh_data, "vertices");

  char* normals_id = xml_node_get_attribute(xml_node_get_child_with_attribute(material_node, "input", "semantic", "NORMAL"), "source") + 1;
  struct XmlNode* normals_data = xml_node_get_child(xml_node_get_child_with_attribute(mesh_data, "source", "id", normals_id), "float_array");
  int count = atoi(xml_node_get_attribute(normals_data, "count"));
  int stride = atoi(xml_node_get_attribute(xml_node_get_child(xml_node_get_child(xml_node_get_child_with_attribute(mesh_data, "source", "id", normals_id), "technique_common"), "accessor"), "stride"));

  char* raw_data = strdup(xml_node_get_data(normals_data));
  char* raw_part = strtok(raw_data, " ");
  for (int normal_num = 0; normal_num < count && raw_part != NULL; normal_num++) {
    vec4 normal = {0};
    for (int dim_num = 0; dim_num < stride; dim_num++) {
      normal.data[dim_num] = atof(raw_part);
      raw_part = strtok(NULL, " ");
    }

    mat4 correction = mat4_rotate(MAT4_IDENTITY, degree_to_radian(-90.0f), (vec3){.data[0] = 1.0f, .data[1] = 0.0f, .data[2] = 0.0f});
    normal = mat4_mul_vec4(correction, normal);
    vec3 normal_corrected = (vec3){.data[0] = normal.data[0], .data[1] = normal.data[1], .data[2] = normal.data[2]};
    vector_push_back(model_data->normals, &normal_corrected);
  }

  free(raw_data);
}

void geometry_loader_read_texture_coordinates(struct ModelData* model_data, struct XmlNode* mesh_data) {
  struct XmlNode* material_node = xml_node_get_child(mesh_data, "polylist");
  if (material_node == NULL)
    material_node = xml_node_get_child(mesh_data, "triangles");
  //if (xml_node_get_child_with_attribute(material_node, "input", "semantic", "TEXCOORD") == NULL)
  //  material_node = xml_node_get_child(mesh_data, "vertices");

  char* tex_coords_id = xml_node_get_attribute(xml_node_get_child_with_attribute(material_node, "input", "semantic", "TEXCOORD"), "source") + 1;
  struct XmlNode* tex_coords_data = xml_node_get_child(xml_node_get_child_with_attribute(mesh_data, "source", "id", tex_coords_id), "float_array");
  int count = atoi(xml_node_get_attribute(tex_coords_data, "count"));
  int stride = atoi(xml_node_get_attribute(xml_node_get_child(xml_node_get_child(xml_node_get_child_with_attribute(mesh_data, "source", "id", tex_coords_id), "technique_common"), "accessor"), "stride"));

  char* raw_data = strdup(xml_node_get_data(tex_coords_data));
  char* raw_part = strtok(raw_data, " ");
  for (int tex_coord_num = 0; tex_coord_num < count && raw_part != NULL; tex_coord_num++) {
    vec4 tex_coord = {0};
    for (int dim_num = 0; dim_num < stride; dim_num++) {
      tex_coord.data[dim_num] = atof(raw_part);
      raw_part = strtok(NULL, " ");
    }

    vector_push_back(model_data->tex_coords, &tex_coord);
  }

  free(raw_data);
}

void geometry_loader_read_colors(struct ModelData* model_data, struct XmlNode* mesh_data) {
  struct XmlNode* material_node = xml_node_get_child(mesh_data, "polylist");
  if (material_node == NULL)
    material_node = xml_node_get_child(mesh_data, "triangles");
  //if (xml_node_get_child_with_attribute(material_node, "input", "semantic", "COLOR") == NULL)
  //  material_node = xml_node_get_child(mesh_data, "vertices");

  struct XmlNode* colors_location = xml_node_get_child_with_attribute(material_node, "input", "semantic", "COLOR");
  if (colors_location == NULL) {
    vec3 color = (vec3){.r = 0.0, .g = 0.0, .b = 0.0};
    vector_push_back(model_data->colors, &color);
    return;
  }
  char* colors_id = xml_node_get_attribute(colors_location, "source") + 1;
  struct XmlNode* colors_data = xml_node_get_child(xml_node_get_child_with_attribute(mesh_data, "source", "id", colors_id), "float_array");
  int count = atoi(xml_node_get_attribute(colors_data, "count"));
  int stride = atoi(xml_node_get_attribute(xml_node_get_child(xml_node_get_child(xml_node_get_child_with_attribute(mesh_data, "source", "id", colors_id), "technique_common"), "accessor"), "stride"));

  char* raw_data = strdup(xml_node_get_data(colors_data));
  char* raw_part = strtok(raw_data, " ");
  while (raw_part != NULL) {
    vec4 color = {0};
    for (int type_num = 0; type_num < stride; type_num++) {
      switch (type_num) {
        case 0:
          color.r = atof(raw_part);
          break;
        case 1:
          color.g = atof(raw_part);
          break;
        case 2:
          color.b = atof(raw_part);
          break;
        case 3:
          color.a = atof(raw_part);
          break;
      }
      raw_part = strtok(NULL, " ");
    }

    vector_push_back(model_data->colors, &color);
  }
  free(raw_data);
}

// Note: All collada models must follow this format and can only be 1 object
// Maybe add error checking to ignore any extras
void geometry_loader_assemble_vertices(struct ModelData* model_data, struct XmlNode* mesh_data) {
  struct XmlNode* poly = xml_node_get_child(mesh_data, "polylist");
  if (poly == NULL)
    poly = xml_node_get_child(mesh_data, "triangles");

  struct XmlNode* index_data = xml_node_get_child(poly, "p");
  int type_count = array_list_size(xml_node_get_children(poly, "input"));

  char* raw_data = strdup(xml_node_get_data(index_data));
  char* raw_part = strtok(raw_data, " ");
  while (raw_part != NULL) {
    int position_index = 0, normal_index = 0, tex_coord_index = 0, color_index = 0;

    for (int type_num = 0; type_num < type_count; type_num++) {
      switch (type_num) {
        case 0:
          position_index = atoi(raw_part);
          break;
        case 1:
          normal_index = atoi(raw_part);
          break;
        case 2:
          tex_coord_index = atoi(raw_part);
          break;
        case 3:
          color_index = atoi(raw_part);
          break;
      }
      raw_part = strtok(NULL, " ");
    }
    geometry_loader_process_vertex(model_data, position_index, normal_index, tex_coord_index, color_index);
  }
  free(raw_data);
}

void geometry_loader_process_vertex(struct ModelData* model_data, int position_index, int normal_index, int tex_coord_index, int color_index) {
  struct RawVertexModel* current_vertex = (struct RawVertexModel*)vector_get(model_data->vertices, position_index);
  if (raw_vertex_model_is_set(current_vertex) == false) {
    current_vertex->texture_index = tex_coord_index;
    current_vertex->normal_index = normal_index;
    current_vertex->color_index = color_index;
    vector_push_back(model_data->indices, &position_index);
  } else
    geometry_loader_deal_with_already_processed_vertex(model_data, current_vertex, tex_coord_index, normal_index, color_index);
}

float geometry_loader_convert_data_to_arrays(struct ModelData* model_data, struct Mesh* model_mesh, bool animated) {
  float furthest_point = 0.0f;
  for (int vertex_num = 0; vertex_num < vector_size(model_data->vertices); vertex_num++) {
    struct RawVertexModel* current_vertex = (struct RawVertexModel*)vector_get(model_data->vertices, vertex_num);
    if (current_vertex->length > furthest_point)
      furthest_point = current_vertex->length;

    vec3 model_position = current_vertex->position;
    vec3 model_normal = *(vec3*)vector_get(model_data->normals, current_vertex->normal_index);
    vec3 model_color = *(vec3*)vector_get(model_data->colors, current_vertex->color_index);
    vec2 model_tex_coord = *(vec2*)vector_get(model_data->tex_coords, current_vertex->texture_index);
    model_tex_coord.v = 1.0f - model_tex_coord.v;

    if (animated) {
      struct VertexSkinData* model_weights = current_vertex->weights_data;

      ivec3 joint_ids = *(ivec3*)model_weights->joint_ids->items;
      vec3 joint_weights = *(vec3*)model_weights->weights->items;

      mesh_model_assign_vertex(model_mesh->vertices, model_position.x, model_position.y, model_position.z, model_normal.data[0], model_normal.data[1], model_normal.data[2], model_tex_coord.u, model_tex_coord.v, model_color.r, model_color.g, model_color.b, joint_ids.id0, joint_ids.id1, joint_ids.id2, joint_weights.data[0], joint_weights.data[1], joint_weights.data[2]);
    } else
      mesh_model_static_assign_vertex(model_mesh->vertices, model_position.x, model_position.y, model_position.z, model_normal.data[0], model_normal.data[1], model_normal.data[2], model_tex_coord.u, model_tex_coord.v, model_color.r, model_color.g, model_color.b);
  }

  return furthest_point;
}

void geometry_loader_deal_with_already_processed_vertex(struct ModelData* model_data, struct RawVertexModel* previous_vertex, int new_texture_index, int new_normal_index, int new_color_index) {
  if (raw_vertex_model_has_same_texture_and_normal(previous_vertex, new_texture_index, new_normal_index)) {
    vector_push_back(model_data->indices, &previous_vertex->index);
  } else {
    struct RawVertexModel* another_vertex = previous_vertex->duplicate_vertex;
    if (another_vertex != NULL) {
      geometry_loader_deal_with_already_processed_vertex(model_data, another_vertex, new_texture_index, new_normal_index, new_color_index);
    } else {
      struct RawVertexModel duplicate_vertex = {0};
      raw_vertex_model_init(&duplicate_vertex, vector_size(model_data->vertices), previous_vertex->position, previous_vertex->weights_data);
      duplicate_vertex.texture_index = new_texture_index;
      duplicate_vertex.normal_index = new_normal_index;
      duplicate_vertex.color_index = new_color_index;
      vector_push_back(model_data->vertices, &duplicate_vertex);
      vector_push_back(model_data->indices, &duplicate_vertex.index);
      // TODO: Watch could cause problems
      previous_vertex->duplicate_vertex = (struct RawVertexModel*)vector_get(model_data->vertices, vector_size(model_data->vertices));
    }
  }
}

void geometry_loader_remove_unused_vertices(struct ModelData* model_data) {
  for (int vertex_num = 0; vertex_num < vector_size(model_data->vertices); vertex_num++) {
    struct RawVertexModel* vertex = (struct RawVertexModel*)vector_get(model_data->vertices, vertex_num);
    if (raw_vertex_model_is_set(vertex) == false) {
      vertex->texture_index = 0;
      vertex->normal_index = 0;
    }
  }
}
