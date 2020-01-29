#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform AtmosphericScatteringUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
} ubo;

layout(binding = 1) uniform AtmosphericScatteringUniformBufferObjectSettings {
  float exposure;
  vec3 white_point;
  vec3 earth_center;
  vec3 sun_direction;
  vec2 sun_size;
} ubos;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitTangent;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out vec3 outFragPos;
layout(location = 2) out vec3 outNormal;

void main() {
  vec4 world_pos = ubo.model * vec4(inPosition, 1.0f);
  outFragPos = world_pos.xyz;

  mat3 model_mat = mat3(ubo.model);

  outNormal = normalize(model_mat * inNormal);
  outTexCoord = inTexCoord;
  
  gl_Position = ubo.proj * ubo.view * world_pos;
}
