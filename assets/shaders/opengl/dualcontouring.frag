#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform DualContouringUniformBufferObject {
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

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 FragPos;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 normalColor;

void main() {
  normalColor = vec4(normalize(inNormal), 1.0);

  if (normalColor.g < -0.85)
    outColor = vec4(0.75,1,0.2,1);
  else
    outColor = vec4(0.5,0.5,0.5,1.0);

  // ambient
  float ambient_intensity = 0.2;
  vec3 ambient = ambient_intensity * lighting.ambient_color;

  // diffuse 
  vec3 norm = -normalize(inNormal);
  vec3 lightDir = normalize(lighting.direction - FragPos);
  float diffuse_intensity = max(dot(norm, lightDir), 0.0);
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
  float specularStrength = 0.25;
  vec3 viewDir = normalize(dcubo.camera_pos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);  
  float specular_intensity = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  specular_intensity = step(0.98, specular_intensity);
  vec3 specular = specularStrength * specular_intensity * lighting.specular_colour;  
    
  vec3 result = (ambient + diffuse + specular);
  outColor *= vec4(result, 1.0);

  /*// ambient
  float ambient_intensity = 0.2;
  vec3 ambient = ambient_intensity * lighting.ambient_color;

  // diffuse 
  vec3 norm = normalize(inNormal);
  vec3 lightDir = normalize(lighting.direction - FragPos);
  float diffuse_intensity = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diffuse_intensity * lighting.diffuse_colour;
    
  // specular
  // TODO: This value should be loaded from material
  float specularStrength = 0.25;
  vec3 viewDir = normalize(dcubo.camera_pos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);  
  float specular_intensity = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = specularStrength * specular_intensity * lighting.specular_colour;  
    
  vec3 result = (ambient + diffuse + specular);

  float total_intensity = ambient_intensity + diffuse_intensity + specular_intensity;

  for (int channel_num = 0; channel_num < 3; channel_num++)
  {
    if (total_intensity > 0.75) 
      result[channel_num] = 1.0;
    else if (total_intensity > 0.15) 
      result[channel_num] = 0.6;
    else if (total_intensity > 0.1) 
      result[channel_num] = 0.3;
    else 
      result[channel_num] = 0.1;
  }

  outColor *= vec4(result, 1.0);*/
}
