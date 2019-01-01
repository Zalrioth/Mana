#version 330

in vec2 outTexCoord;
in vec3 mvPos;
layout (location = 0) out vec4 fragColor;

uniform sampler2D texture_sampler;
uniform vec3 sunLight;

void main()
{
    fragColor = vec4(sunLight, 1) * texture(texture_sampler, outTexCoord);
}