#version 410 core

in vec2 vTexCoord;
layout(location = 0) out vec4 fragColor;

uniform sampler2D uColorTexture;
uniform sampler2D uBloomTexture;

uniform float bloom_factor = 0.8;
//uniform float bloom_factor = 1.0;
uniform float scene_factor = 1.0;

void main(void)
{
    vec4 c = vec4(0.0);

    c += texture(uColorTexture, vTexCoord) * scene_factor;
    c += texture(uBloomTexture, vTexCoord) * bloom_factor;

    fragColor = c;
}