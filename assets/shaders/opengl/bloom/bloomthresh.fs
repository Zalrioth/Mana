#version 410 core

in vec2 vTexCoord;
layout(location = 0) out vec3 fragColor;

uniform sampler2D uColorTexture;

// Material properties
uniform float bloom_thresh_min = 0.8;
uniform float bloom_thresh_max = 1.2;

void main(void)
{
    vec3 color = texture(uColorTexture, vTexCoord).rgb;
    float Y = dot(color, vec3(0.299, 0.587, 0.144));
    //float Y = dot(color, vec3(0.2126, 0.7152, 0.0722));

    fragColor = color * 4.0 * smoothstep(bloom_thresh_min, bloom_thresh_max, Y);
}