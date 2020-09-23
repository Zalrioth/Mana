#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform ManifoldDualContouringUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
  vec3 camera_pos;
} dcubo;

layout(binding = 1) uniform Lighting {
  vec3 direction;
  vec3 ambient_color;
  vec3 diffuse_colour;
  vec3 specular_colour;
} lighting;

layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec3 in_frag_pos;

layout(location = 0) out vec4 out_frag_color;
layout(location = 1) out vec4 out_normal_color;

void main() {
  out_normal_color = vec4(normalize(in_normal), 1.0);

  if (out_normal_color.g < -0.85)
    out_frag_color = vec4(0.75,1,0.2,1);
  else
    out_frag_color = vec4(0.5,0.5,0.5,1.0);

  // ambient
  float ambient_intensity = 0.2;
  vec3 ambient = ambient_intensity * lighting.ambient_color;

  // diffuse 
  vec3 norm = -normalize(in_normal);
  vec3 light_dir = normalize(lighting.direction - in_frag_pos);
  float diffuse_intensity = max(dot(norm, light_dir), 0.0);
  if (diffuse_intensity >= 0.8)
    diffuse_intensity = 1.0;
  else if (diffuse_intensity >= 0.6)
    diffuse_intensity = 0.6;
  else if (diffuse_intensity >= 0.3)
    diffuse_intensity = 0.3;
  else
    diffuse_intensity = 0.0;
  vec3 diffuse = diffuse_intensity * lighting.diffuse_colour;
    
  // specular
  // TODO: This value should be loaded from material
  float specular_strength = 0.25;
  vec3 view_dir = normalize(dcubo.camera_pos - in_frag_pos);
  vec3 reflect_dir = reflect(-light_dir, norm);  
  float specular_intensity = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
  specular_intensity = step(0.98, specular_intensity);
  vec3 specular = specular_strength * specular_intensity * lighting.specular_colour;  
    
  vec3 result = (ambient + diffuse + specular);
  out_frag_color *= vec4(result, 1.0);
}
