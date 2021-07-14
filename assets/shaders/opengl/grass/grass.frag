#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_color;

layout(location = 0) out vec4 out_frag_color;
layout(location = 1) out vec4 out_normal_color;

void main() {
  out_frag_color = vec4(in_color, 1.0);
  out_normal_color = vec4(in_color, 1.0);
}
