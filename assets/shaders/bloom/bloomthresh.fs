#version 410 core

in vec2 vTexCoord;
out vec4 color1;

uniform sampler2D hdr_image;

// Material properties
uniform float bloom_thresh_min = 0.8;
uniform float bloom_thresh_max = 1.2;

void main(void)
{
    vec3 color = texture(hdr_image, vTexCoord).rgb;
    float Y = dot(color, vec3(0.299, 0.587, 0.144));
    //float Y = dot(color, vec3(0.2126, 0.7152, 0.0722));

    color = color * 4.0 * smoothstep(bloom_thresh_min, bloom_thresh_max, Y);

    color1 = vec4(color, 1);
}