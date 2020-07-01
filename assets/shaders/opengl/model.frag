#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "modelcommon.glsl"

const vec2 lightBias = vec2(0.7, 0.6); //just indicates the balance between diffuse and ambient lighting
const int MAX_JOINTS = 50; //max joints allowed in a skeleton
const int MAX_WEIGHTS = 3; //max number of joints that can affect a vertex

layout(binding = 0) uniform ModelUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
  vec3 camera_pos;
	mat4 jointTransforms[MAX_JOINTS];
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
  vec4 diffuseColour = texture(diffuseMap, textureCoords);	
  
  // Normals not flipped
  vec3 norm = normalize(inNormal);
  outColor = calculate_pbr(diffuseColour, norm, FragPos, ubo.camera_pos, lighting.light);
  normalColor = vec4(norm, 1.0);
}