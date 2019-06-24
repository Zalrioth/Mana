#version 410 core
//https://github.com/lragnarsson/Emerald-Engine/blob/master/src/shaders/grass.frag

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in vec3 groundNormals;

//layout (location = 0) out vec3 g_position;
//layout (location = 1) out vec4 g_normal_shininess;
//layout (location = 2) out vec4 g_albedo_specular;

layout(location = 0) out vec4 g_albedo_specular;
layout(location = 1) out vec4 g_normal_shininess;
layout(location = 3) out vec4 g_position;

uniform float shininess;

uniform sampler2D wind_map;
uniform sampler2D diffuse_map;

uniform vec3 color;

const float SPECULAR = 0.1;

void main()
{
    g_position.rgb = FragPos;
    g_normal_shininess = vec4(Normal, 1.0);
    g_albedo_specular = vec4(color, 1.0);

    //g_normal_shininess.rgb = Normal;
    //g_normal_shininess.a = shininess;
    //g_albedo_specular.rgb = mix(texture(diffuse_map, TexCoord).rgb, 0.8 * normalize(vec3(0.32, 0.7, 0.2)), 0.3);
    //g_albedo_specular.a = SPECULAR;
}
