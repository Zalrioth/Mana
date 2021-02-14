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
  vec3 light_dir = normalize(light.direction - frag_pos);
  float diffuse_intensity = max(dot(normals, light_dir), 0.0);
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
  vec3 reflectDir = reflect(-light_dir, normals);  
  float specular_intensity = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  specular_intensity = step(0.98, specular_intensity);
  vec3 specular = specularStrength * specular_intensity * light.specular_colour;

  // rim lighting kinda working but will need to look at later
  //float n_dot_l = dot(light.direction, normals);
  //float rim_dot = 1.0 - dot(viewDir, normals);
  //vec4 rim_color = vec4(1.0, 1.0, 1.0, 1.0);
  //float rim_amount = 0.716; // Range is 0.0 - 1.0
  //float rim_threshold = 0.1; // Range is 0.0 - 1.0
  //float rim_intensity = rim_dot * pow(n_dot_l, rim_threshold);
  //rim_intensity = smoothstep(rim_amount - 0.01, rim_amount + 0.01, rim_intensity);
  //vec4 rim = rim_intensity * rim_color;
  //
  //vec3 result = (ambient + diffuse + specular + rim.rgb);
  vec3 result = (ambient + diffuse + specular);
  return albedo * vec4(result, 1.0);
}

// Smoothstep can be used to buff out hard edges, FXAA already does a pretty good job
//vec4 calculate_pbr(vec4 albedo, vec3 normals, vec3 frag_pos, vec3 camera_pos, Light light) {
//    // ambient
//  float ambient_intensity = 0.2;
//  vec3 ambient = ambient_intensity * light.ambient_color;
//
//  // diffuse
//  vec3 lightDir = normalize(light.direction - frag_pos);
//  float diffuse_intensity = max(dot(normals, lightDir), 0.0);
//  if (diffuse_intensity >= 0.8)
//    diffuse_intensity = 1.0;
//  if (diffuse_intensity >= 0.795)
//    diffuse_intensity = (smoothstep(0.795, 0.8, diffuse_intensity) / (10.0 / 4.0)) + 0.6;
//  else if (diffuse_intensity >= 0.6)
//    diffuse_intensity = 0.6;
//  else if (diffuse_intensity >= 0.595)
//    diffuse_intensity = (smoothstep(0.595, 0.6, diffuse_intensity) / (10.0 / 7.0)) + 0.3;
//  else if (diffuse_intensity >= 0.3)
//    diffuse_intensity = 0.3;
//  else if (diffuse_intensity >= 0.295)
//    diffuse_intensity = smoothstep(0.295, 0.3, diffuse_intensity) - 0.7;
//  else
//    diffuse_intensity = 0.0;
//  vec3 diffuse = diffuse_intensity * light.diffuse_colour;
//    
//  // specular
//  float specularStrength = 0.25;
//  vec3 viewDir = normalize(camera_pos - frag_pos);
//  vec3 reflectDir = reflect(-lightDir, normals);  
//  float specular_intensity = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//  specular_intensity = step(0.98, specular_intensity);
//  vec3 specular = specularStrength * specular_intensity * light.specular_colour;  
//    
//  vec3 result = (ambient + diffuse + specular);
//  return albedo * vec4(result, 1.0);
//}
