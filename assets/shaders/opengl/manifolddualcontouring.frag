#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "modelcommon.glsl"

layout(binding = 0) uniform ManifoldDualContouringUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
  vec3 camera_pos;
} dcubo;
layout(binding = 1) uniform Lighting {
  Light light;
} lighting;

layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec3 in_frag_pos;

layout(location = 0) out vec4 out_frag_color;
layout(location = 1) out vec4 out_normal_color;

void main() {
  out_normal_color = vec4(normalize(in_normal), 1.0);

  if (out_normal_color.g < -0.85)
    out_frag_color = vec4(0.75,1,0.2,1);
  else
    out_frag_color = vec4(0.5,0.5,0.5,1.0);

  // Normals flipped
  vec3 norm = -normalize(in_normal);
  out_frag_color = calculate_pbr(out_frag_color, norm, in_frag_pos, dcubo.camera_pos, lighting.light);
  out_normal_color = vec4(norm, 1.0);
}
