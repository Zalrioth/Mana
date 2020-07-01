#version 450
#extension GL_ARB_separate_shader_objects : enable

const int MAX_JOINTS = 50;//max joints allowed in a skeleton
const int MAX_WEIGHTS = 3;//max number of joints that can affect a vertex

layout(binding = 0) uniform ModelUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
	vec3 camera_pos;
	mat4 jointTransforms[MAX_JOINTS];
} ubo;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 color;
layout(location = 4) in ivec3 joints_ids;
layout(location = 5) in vec3 weights;

layout(location = 0) out vec2 textureCoords;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outColor;
layout(location = 3) out vec3 FragPos;

void main(void){
	vec4 totalLocalPos = vec4(0.0);
	vec4 totalNormal = vec4(0.0);
	
	for(int i = 0; i < MAX_WEIGHTS; i++){
		mat4 jointTransform = ubo.jointTransforms[joints_ids[i]];
		vec4 posePosition = jointTransform * vec4(position, 1.0);
		totalLocalPos += posePosition * weights[i];
		
		vec4 worldNormal = jointTransform * vec4(normal, 0.0);
		totalNormal += worldNormal * weights[i];
	}

	gl_Position = ubo.proj * ubo.view * ubo.model * totalLocalPos;
	outNormal = totalNormal.xyz;
	textureCoords = tex_coord;

	outColor = color;
	FragPos = vec3(ubo.model * vec4(position, 1.0));
}