#version 450
#extension GL_ARB_separate_shader_objects : enable

const vec2 lightBias = vec2(0.7, 0.6);//just indicates the balance between diffuse and ambient lighting
const int MAX_JOINTS = 50;//max joints allowed in a skeleton
const int MAX_WEIGHTS = 3;//max number of joints that can affect a vertex

layout(binding = 0) uniform ModelUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
	mat4 jointTransforms[MAX_JOINTS];
	vec3 lightDirection;
} ubo;
layout(binding = 1) uniform sampler2D diffuseMap;

layout(location = 0) in vec2 textureCoords;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 normalColor;

void main(void){
	vec4 diffuseColour = texture(diffuseMap, textureCoords);		
	vec3 unitNormal = normalize(inNormal);
	float diffuseLight = max(dot(-ubo.lightDirection, unitNormal), 0.0) * lightBias.x + lightBias.y;
	outColor = diffuseColour * diffuseLight;
	outColor = vec4(1.0, 0.0, 0.0, 1.0);
}