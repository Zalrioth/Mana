#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform AtmosphericScatteringUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
} ubo;

layout(binding = 1) uniform AtmosphericScatteringUniformBufferObjectSettings {
  float exposure;
  vec3 white_point;
  vec3 earth_center;
  vec3 sun_direction;
  vec2 sun_size;
} ubos;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_tex_coord;
layout(location = 3) in vec3 in_tangent;
layout(location = 4) in vec3 in_bit_tangent;

layout(location = 0) out vec2 out_tex_coord;
layout(location = 1) out vec3 out_frag_pos;
layout(location = 2) out vec3 out_normal;

void main() {
  vec4 world_pos = ubo.model * vec4(in_position, 1.0f);
  out_frag_pos = world_pos.xyz;

  mat3 model_mat = mat3(ubo.model);

  out_normal = normalize(model_mat * in_normal);
  out_tex_coord = in_tex_coord;
  
  gl_Position = ubo.proj * ubo.view * world_pos;
}
