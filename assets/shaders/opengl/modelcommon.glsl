struct Light {
  vec3 direction;
  vec3 ambient_color;
  vec3 diffuse_colour;
  vec3 specular_colour;
};

vec4 calculate_pbr(vec4 albedo, vec3 normals, vec3 frag_pos, vec3 camera_pos, Light light) {
    // ambient
  float ambient_intensity = 0.2;
  vec3 ambient = ambient_intensity * light.ambient_color;

  // diffuse
  vec3 lightDir = normalize(light.direction - frag_pos);
  float diffuse_intensity = max(dot(normals, lightDir), 0.0);
  if (diffuse_intensity >= 0.8)
    diffuse_intensity = 1.0;
  else if (diffuse_intensity >= 0.6)
    diffuse_intensity = 0.6;
  else if (diffuse_intensity >= 0.3)
    diffuse_intensity = 0.3;
  else
    diffuse_intensity = 0.0;
  vec3 diffuse = diffuse_intensity * light.diffuse_colour;
    
  // specular
  float specularStrength = 0.25;
  vec3 viewDir = normalize(camera_pos - frag_pos);
  vec3 reflectDir = reflect(-lightDir, normals);  
  float specular_intensity = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  specular_intensity = step(0.98, specular_intensity);
  vec3 specular = specularStrength * specular_intensity * light.specular_colour;  
    
  vec3 result = (ambient + diffuse + specular);
  return albedo * vec4(result, 1.0);
}