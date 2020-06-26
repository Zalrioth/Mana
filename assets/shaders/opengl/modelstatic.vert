#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform ModelStaticUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
	vec3 lightDirection;
} ubo;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 color;

layout(location = 0) out vec2 textureCoords;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outColor;

void main(void){
	vec4 totalLocalPos = vec4(0.0);
	vec4 totalNormal = vec4(0.0);
	
  gl_Position = ubo.proj * ubo.view * ubo.model * vec4(position, 1.0);
	outNormal = totalNormal.xyz;
	textureCoords = tex_coord;

	outColor = color;
}