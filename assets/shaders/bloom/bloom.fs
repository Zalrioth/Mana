#version 410 core

uniform sampler2D hdr_image;
uniform sampler2D bloom_image;

uniform float bloom_factor = 0.8;
//uniform float bloom_factor = 1.0;
uniform float scene_factor = 1.0;

in vec2 vTexCoord;
out vec4 color;

void main(void)
{
    vec4 c = vec4(0.0);

    c += texture(hdr_image, vTexCoord) * scene_factor;
    c += texture(bloom_image, vTexCoord) * bloom_factor;

    color = c;
}