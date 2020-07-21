#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "modelcommon.glsl"

const int MAX_JOINTS = 50;
const int MAX_WEIGHTS = 3;

layout(binding = 0) uniform ModelUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
  vec3 camera_pos;
	mat4 joint_transforms[MAX_JOINTS];
} ubo;
layout(binding = 1) uniform Lighting {
  Light light;
} lighting;
layout(binding = 2) uniform sampler2D diffuse_map;

layout(location = 0) in vec2 in_tex_coord;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_color;
layout(location = 3) in vec3 in_frag_pos;

layout(location = 0) out vec4 out_frag_color;
layout(location = 1) out vec4 out_normal_color;

void main(void){	
  vec4 diffuse_color = texture(diffuse_map, in_tex_coord);	
  
  // Normals not flipped
  vec3 norm = normalize(in_normal);
  out_frag_color = calculate_pbr(diffuse_color, norm, in_frag_pos, ubo.camera_pos, lighting.light);
  out_normal_color = vec4(norm, 1.0);
}