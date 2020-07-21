#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform ModelStaticUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
	vec3 camera_pos;
} ubo;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_tex_coord;
layout(location = 3) in vec3 in_color;

layout(location = 0) out vec2 out_tex_coord;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec3 out_color;
layout(location = 3) out vec3 out_frag_pos;

void main(void){
  gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_position, 1.0);
	out_normal = in_normal;
	out_tex_coord = in_tex_coord;

	out_color = in_color;
	out_frag_pos = vec3(ubo.model * vec4(in_position, 1.0));
}