#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform DualContouringUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
} ubo;

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 normalColor;

void main() {
  outColor = vec4(1.0, 0.0, 0.0, 1.0);
  normalColor = vec4(normalize(inNormal), 1.0);
}
