#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform SpriteUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
  vec3 frame_pos;
} ubo;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_tex_coord;

layout(location = 0) out vec2 frag_tex_coord;

void main() {
  gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_position, 1.0);
  frag_tex_coord = in_tex_coord;
}
