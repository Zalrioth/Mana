#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "modelcommon.glsl"

const vec2 lightBias = vec2(0.7, 0.6); //just indicates the balance between diffuse and ambient lighting

layout(binding = 0) uniform ModelStaticUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
  vec3 camera_pos;
} ubo;
layout(binding = 1) uniform Lighting {
  Light light;
} lighting;
layout(binding = 2) uniform sampler2D diffuseMap;

layout(location = 0) in vec2 textureCoords;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec3 FragPos;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 normalColor;

void main(void){
	bool textured = false;
	//vec4 diffuseColour = vec4(inColor, 1.0);
	//if (textured)
	//	outColor = texture(diffuseMap, textureCoords);	
	//else // Models using vertex colors don't need texture coordinates? Look more into this

  vec4 diffuseColour = texture(diffuseMap, textureCoords);	
  
	//outColor = vec4(inColor, 1.0);
  // Normals not flipped
  vec3 norm = normalize(inNormal);
  outColor = calculate_pbr(diffuseColour, norm, FragPos, ubo.camera_pos, lighting.light);
  normalColor = vec4(norm, 1.0);
}