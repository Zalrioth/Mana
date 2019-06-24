#version 410 core

in vec4 gPosition;
in vec3 mvVertexNormal;
in vec2 TexCoords;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 normalColor;
layout(location = 2) out vec4 positionColor;
layout(location = 3) out vec4 lightScatteringColor;

uniform sampler2D texture_diffuse1;

void main()
{    
    normalColor = vec4(mvVertexNormal, 1.0);
    fragColor = texture(texture_diffuse1, TexCoords);
    positionColor = gPosition;
    lightScatteringColor = vec4(0.0, 0.0, 0.0, fragColor.w);
}