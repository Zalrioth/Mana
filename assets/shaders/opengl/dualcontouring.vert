#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform DualContouringUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
  vec3 camera;
} dcubo;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec3 out_frag_pos;

void main() {
  gl_Position = dcubo.proj * dcubo.view * dcubo.model * vec4(in_position, 1.0);
  //mat3 normal_matrix = transpose(inverse(mat3(dcubo.view * dcubo.model)));
  out_normal = in_normal;

  out_frag_pos = vec3(dcubo.model * vec4(in_position, 1.0));
}
