#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform AtmosphericScatteringUniformBufferObject {
  mat4 model_from_view;
  vec3 camera;
} ubo;

layout(binding = 1) uniform AtmosphericScatteringUniformBufferObjectSettings {
  mat4 view_from_clip;
  float exposure;
  vec3 white_point;
  vec3 earth_center;
  vec3 sun_direction;
  vec2 sun_size;
} ubos;
