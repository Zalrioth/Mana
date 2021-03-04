#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform SpriteUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
  vec3 frame_pos;
} ubo;
layout(binding = 1) uniform sampler2D tex_sampler;

layout(location = 0) in vec2 in_tex_coord;

layout(location = 0) out vec4 out_frag_color;
layout(location = 1) out vec4 out_normal_color;

void main() {
  // Check for direction facing
  if (ubo.frame_pos.p < 0)
    out_frag_color = texture(tex_sampler, in_tex_coord + ubo.frame_pos.st);
  else
    out_frag_color = texture(tex_sampler, vec2(ubo.frame_pos.s - in_tex_coord.s, in_tex_coord.t + ubo.frame_pos.t));
}
