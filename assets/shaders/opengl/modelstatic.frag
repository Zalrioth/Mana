#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "modelcommon.glsl"

layout(binding = 0) uniform ModelStaticUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
  vec3 camera_pos;
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
	bool textured = false;
	//vec4 diffuseColour = vec4(inColor, 1.0);
	//if (textured)
	//	outColor = texture(diffuseMap, textureCoords);	
	//else // Models using vertex colors don't need texture coordinates? Look more into this

  vec4 diffuse_color = texture(diffuse_map, in_tex_coord);	
  
	//outColor = vec4(inColor, 1.0);
  // Normals not flipped
  vec3 norm = normalize(in_normal);
  out_frag_color = calculate_pbr(diffuse_color, norm, in_frag_pos, ubo.camera_pos, lighting.light);
  out_normal_color = vec4(norm, 1.0);
}