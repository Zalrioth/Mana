#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform GrassUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
} ubo;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_color;
layout(location = 3) in vec3 in_wind_pos;
layout(location = 4) in float trample_val;

layout(location = 0) out vec3 frag_color;

void main() {
  gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_position, 1.0);
  frag_color = in_color;
}
