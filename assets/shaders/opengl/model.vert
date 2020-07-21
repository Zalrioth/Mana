#version 450
#extension GL_ARB_separate_shader_objects : enable

const int MAX_JOINTS = 50;
const int MAX_WEIGHTS = 3;

layout(binding = 0) uniform ModelUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
	vec3 camera_pos;
	mat4 joint_transforms[MAX_JOINTS];
} ubo;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 color;
layout(location = 4) in ivec3 joints_ids;
layout(location = 5) in vec3 weights;

layout(location = 0) out vec2 out_texture_coords;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec3 out_frag_color;
layout(location = 3) out vec3 out_frag_pos;

void main(void){
	vec4 total_local_pos = vec4(0.0);
	vec4 total_normal = vec4(0.0);
	
	for(int i = 0; i < MAX_WEIGHTS; i++){
		mat4 joint_transform = ubo.joint_transforms[joints_ids[i]];
		vec4 pose_position = joint_transform * vec4(position, 1.0);
		total_local_pos += pose_position * weights[i];
		
		vec4 world_normal = joint_transform * vec4(normal, 0.0);
		total_normal += world_normal * weights[i];
	}

	gl_Position = ubo.proj * ubo.view * ubo.model * total_local_pos;
	out_normal = total_normal.xyz;
	out_texture_coords = tex_coord;

	out_frag_color = color;
	out_frag_pos = vec3(ubo.model * vec4(position, 1.0));
}